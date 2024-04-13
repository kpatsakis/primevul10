__generic_file_splice_read(struct file *in, loff_t *ppos,
			   struct pipe_inode_info *pipe, size_t len,
			   unsigned int flags)
{
	struct address_space *mapping = in->f_mapping;
	unsigned int loff, nr_pages;
	struct page *pages[PIPE_BUFFERS];
	struct partial_page partial[PIPE_BUFFERS];
	struct page *page;
	pgoff_t index, end_index;
	loff_t isize;
	size_t total_len;
	int error, page_nr;
	struct splice_pipe_desc spd = {
		.pages = pages,
		.partial = partial,
		.flags = flags,
		.ops = &page_cache_pipe_buf_ops,
	};

	index = *ppos >> PAGE_CACHE_SHIFT;
	loff = *ppos & ~PAGE_CACHE_MASK;
	nr_pages = (len + loff + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

	if (nr_pages > PIPE_BUFFERS)
		nr_pages = PIPE_BUFFERS;

	/*
	 * Initiate read-ahead on this page range. however, don't call into
	 * read-ahead if this is a non-zero offset (we are likely doing small
	 * chunk splice and the page is already there) for a single page.
	 */
	if (!loff || nr_pages > 1)
		page_cache_readahead(mapping, &in->f_ra, in, index, nr_pages);

	/*
	 * Now fill in the holes:
	 */
	error = 0;
	total_len = 0;

	/*
	 * Lookup the (hopefully) full range of pages we need.
	 */
	spd.nr_pages = find_get_pages_contig(mapping, index, nr_pages, pages);

	/*
	 * If find_get_pages_contig() returned fewer pages than we needed,
	 * allocate the rest.
	 */
	index += spd.nr_pages;
	while (spd.nr_pages < nr_pages) {
		/*
		 * Page could be there, find_get_pages_contig() breaks on
		 * the first hole.
		 */
		page = find_get_page(mapping, index);
		if (!page) {
			/*
			 * Make sure the read-ahead engine is notified
			 * about this failure.
			 */
			handle_ra_miss(mapping, &in->f_ra, index);

			/*
			 * page didn't exist, allocate one.
			 */
			page = page_cache_alloc_cold(mapping);
			if (!page)
				break;

			error = add_to_page_cache_lru(page, mapping, index,
					      mapping_gfp_mask(mapping));
			if (unlikely(error)) {
				page_cache_release(page);
				if (error == -EEXIST)
					continue;
				break;
			}
			/*
			 * add_to_page_cache() locks the page, unlock it
			 * to avoid convoluting the logic below even more.
			 */
			unlock_page(page);
		}

		pages[spd.nr_pages++] = page;
		index++;
	}

	/*
	 * Now loop over the map and see if we need to start IO on any
	 * pages, fill in the partial map, etc.
	 */
	index = *ppos >> PAGE_CACHE_SHIFT;
	nr_pages = spd.nr_pages;
	spd.nr_pages = 0;
	for (page_nr = 0; page_nr < nr_pages; page_nr++) {
		unsigned int this_len;

		if (!len)
			break;

		/*
		 * this_len is the max we'll use from this page
		 */
		this_len = min_t(unsigned long, len, PAGE_CACHE_SIZE - loff);
		page = pages[page_nr];

		/*
		 * If the page isn't uptodate, we may need to start io on it
		 */
		if (!PageUptodate(page)) {
			/*
			 * If in nonblock mode then dont block on waiting
			 * for an in-flight io page
			 */
			if (flags & SPLICE_F_NONBLOCK)
				break;

			lock_page(page);

			/*
			 * page was truncated, stop here. if this isn't the
			 * first page, we'll just complete what we already
			 * added
			 */
			if (!page->mapping) {
				unlock_page(page);
				break;
			}
			/*
			 * page was already under io and is now done, great
			 */
			if (PageUptodate(page)) {
				unlock_page(page);
				goto fill_it;
			}

			/*
			 * need to read in the page
			 */
			error = mapping->a_ops->readpage(in, page);
			if (unlikely(error)) {
				/*
				 * We really should re-lookup the page here,
				 * but it complicates things a lot. Instead
				 * lets just do what we already stored, and
				 * we'll get it the next time we are called.
				 */
				if (error == AOP_TRUNCATED_PAGE)
					error = 0;

				break;
			}

			/*
			 * i_size must be checked after ->readpage().
			 */
			isize = i_size_read(mapping->host);
			end_index = (isize - 1) >> PAGE_CACHE_SHIFT;
			if (unlikely(!isize || index > end_index))
				break;

			/*
			 * if this is the last page, see if we need to shrink
			 * the length and stop
			 */
			if (end_index == index) {
				loff = PAGE_CACHE_SIZE - (isize & ~PAGE_CACHE_MASK);
				if (total_len + loff > isize)
					break;
				/*
				 * force quit after adding this page
				 */
				len = this_len;
				this_len = min(this_len, loff);
				loff = 0;
			}
		}
fill_it:
		partial[page_nr].offset = loff;
		partial[page_nr].len = this_len;
		len -= this_len;
		total_len += this_len;
		loff = 0;
		spd.nr_pages++;
		index++;
	}

	/*
	 * Release any pages at the end, if we quit early. 'i' is how far
	 * we got, 'nr_pages' is how many pages are in the map.
	 */
	while (page_nr < nr_pages)
		page_cache_release(pages[page_nr++]);

	if (spd.nr_pages)
		return splice_to_pipe(pipe, &spd);

	return error;
}