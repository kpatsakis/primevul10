static int pipe_to_file(struct pipe_inode_info *pipe, struct pipe_buffer *buf,
			struct splice_desc *sd)
{
	struct file *file = sd->file;
	struct address_space *mapping = file->f_mapping;
	gfp_t gfp_mask = mapping_gfp_mask(mapping);
	unsigned int offset, this_len;
	struct page *page;
	pgoff_t index;
	int ret;

	/*
	 * make sure the data in this buffer is uptodate
	 */
	ret = buf->ops->pin(pipe, buf);
	if (unlikely(ret))
		return ret;

	index = sd->pos >> PAGE_CACHE_SHIFT;
	offset = sd->pos & ~PAGE_CACHE_MASK;

	this_len = sd->len;
	if (this_len + offset > PAGE_CACHE_SIZE)
		this_len = PAGE_CACHE_SIZE - offset;

	/*
	 * Reuse buf page, if SPLICE_F_MOVE is set and we are doing a full
	 * page.
	 */
	if ((sd->flags & SPLICE_F_MOVE) && this_len == PAGE_CACHE_SIZE) {
		/*
		 * If steal succeeds, buf->page is now pruned from the
		 * pagecache and we can reuse it. The page will also be
		 * locked on successful return.
		 */
		if (buf->ops->steal(pipe, buf))
			goto find_page;

		page = buf->page;
		if (add_to_page_cache(page, mapping, index, gfp_mask)) {
			unlock_page(page);
			goto find_page;
		}

		page_cache_get(page);

		if (!(buf->flags & PIPE_BUF_FLAG_LRU))
			lru_cache_add(page);
	} else {
find_page:
		page = find_lock_page(mapping, index);
		if (!page) {
			ret = -ENOMEM;
			page = page_cache_alloc_cold(mapping);
			if (unlikely(!page))
				goto out_ret;

			/*
			 * This will also lock the page
			 */
			ret = add_to_page_cache_lru(page, mapping, index,
						    gfp_mask);
			if (unlikely(ret))
				goto out;
		}

		/*
		 * We get here with the page locked. If the page is also
		 * uptodate, we don't need to do more. If it isn't, we
		 * may need to bring it in if we are not going to overwrite
		 * the full page.
		 */
		if (!PageUptodate(page)) {
			if (this_len < PAGE_CACHE_SIZE) {
				ret = mapping->a_ops->readpage(file, page);
				if (unlikely(ret))
					goto out;

				lock_page(page);

				if (!PageUptodate(page)) {
					/*
					 * Page got invalidated, repeat.
					 */
					if (!page->mapping) {
						unlock_page(page);
						page_cache_release(page);
						goto find_page;
					}
					ret = -EIO;
					goto out;
				}
			} else
				SetPageUptodate(page);
		}
	}

	ret = mapping->a_ops->prepare_write(file, page, offset, offset+this_len);
	if (unlikely(ret)) {
		loff_t isize = i_size_read(mapping->host);

		if (ret != AOP_TRUNCATED_PAGE)
			unlock_page(page);
		page_cache_release(page);
		if (ret == AOP_TRUNCATED_PAGE)
			goto find_page;

		/*
		 * prepare_write() may have instantiated a few blocks
		 * outside i_size.  Trim these off again.
		 */
		if (sd->pos + this_len > isize)
			vmtruncate(mapping->host, isize);

		goto out_ret;
	}

	if (buf->page != page) {
		/*
		 * Careful, ->map() uses KM_USER0!
		 */
		char *src = buf->ops->map(pipe, buf, 1);
		char *dst = kmap_atomic(page, KM_USER1);

		memcpy(dst + offset, src + buf->offset, this_len);
		flush_dcache_page(page);
		kunmap_atomic(dst, KM_USER1);
		buf->ops->unmap(pipe, buf, src);
	}

	ret = mapping->a_ops->commit_write(file, page, offset, offset+this_len);
	if (!ret) {
		/*
		 * Return the number of bytes written and mark page as
		 * accessed, we are now done!
		 */
		ret = this_len;
		mark_page_accessed(page);
		balance_dirty_pages_ratelimited(mapping);
	} else if (ret == AOP_TRUNCATED_PAGE) {
		page_cache_release(page);
		goto find_page;
	}
out:
	page_cache_release(page);
	unlock_page(page);
out_ret:
	return ret;
}