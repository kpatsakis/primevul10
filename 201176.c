static int ext4_journalled_writepage(struct page *page,
				struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	loff_t size = i_size_read(inode);
	loff_t len;

	trace_mark(ext4_journalled_writepage,
		   "dev %s ino %lu page_index %lu",
		   inode->i_sb->s_id, inode->i_ino, page->index);
	J_ASSERT(PageLocked(page));
	if (page->index == size >> PAGE_CACHE_SHIFT)
		len = size & ~PAGE_CACHE_MASK;
	else
		len = PAGE_CACHE_SIZE;

	if (page_has_buffers(page)) {
		/* if page has buffers it should all be mapped
		 * and allocated. If there are not buffers attached
		 * to the page we know the page is dirty but it lost
		 * buffers. That means that at some moment in time
		 * after write_begin() / write_end() has been called
		 * all buffers have been clean and thus they must have been
		 * written at least once. So they are all mapped and we can
		 * happily proceed with mapping them and writing the page.
		 */
		BUG_ON(walk_page_buffers(NULL, page_buffers(page), 0, len, NULL,
					ext4_bh_unmapped_or_delay));
	}

	if (ext4_journal_current_handle())
		goto no_write;

	if (PageChecked(page)) {
		/*
		 * It's mmapped pagecache.  Add buffers and journal it.  There
		 * doesn't seem much point in redirtying the page here.
		 */
		ClearPageChecked(page);
		return __ext4_journalled_writepage(page, wbc);
	} else {
		/*
		 * It may be a page full of checkpoint-mode buffers.  We don't
		 * really know unless we go poke around in the buffer_heads.
		 * But block_write_full_page will do the right thing.
		 */
		return block_write_full_page(page,
						ext4_normal_get_block_write,
						wbc);
	}
no_write:
	redirty_page_for_writepage(wbc, page);
	unlock_page(page);
	return 0;
}