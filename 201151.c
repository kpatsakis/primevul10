static int __mpage_da_writepage(struct page *page,
				struct writeback_control *wbc, void *data)
{
	struct mpage_da_data *mpd = data;
	struct inode *inode = mpd->inode;
	struct buffer_head *bh, *head, fake;
	sector_t logical;

	if (mpd->io_done) {
		/*
		 * Rest of the page in the page_vec
		 * redirty then and skip then. We will
		 * try to to write them again after
		 * starting a new transaction
		 */
		redirty_page_for_writepage(wbc, page);
		unlock_page(page);
		return MPAGE_DA_EXTENT_TAIL;
	}
	/*
	 * Can we merge this page to current extent?
	 */
	if (mpd->next_page != page->index) {
		/*
		 * Nope, we can't. So, we map non-allocated blocks
		 * and start IO on them using writepage()
		 */
		if (mpd->next_page != mpd->first_page) {
			if (mpage_da_map_blocks(mpd) == 0)
				mpage_da_submit_io(mpd);
			/*
			 * skip rest of the page in the page_vec
			 */
			mpd->io_done = 1;
			redirty_page_for_writepage(wbc, page);
			unlock_page(page);
			return MPAGE_DA_EXTENT_TAIL;
		}

		/*
		 * Start next extent of pages ...
		 */
		mpd->first_page = page->index;

		/*
		 * ... and blocks
		 */
		mpd->lbh.b_size = 0;
		mpd->lbh.b_state = 0;
		mpd->lbh.b_blocknr = 0;
	}

	mpd->next_page = page->index + 1;
	logical = (sector_t) page->index <<
		  (PAGE_CACHE_SHIFT - inode->i_blkbits);

	if (!page_has_buffers(page)) {
		/*
		 * There is no attached buffer heads yet (mmap?)
		 * we treat the page asfull of dirty blocks
		 */
		bh = &fake;
		bh->b_size = PAGE_CACHE_SIZE;
		bh->b_state = 0;
		set_buffer_dirty(bh);
		set_buffer_uptodate(bh);
		mpage_add_bh_to_extent(mpd, logical, bh);
		if (mpd->io_done)
			return MPAGE_DA_EXTENT_TAIL;
	} else {
		/*
		 * Page with regular buffer heads, just add all dirty ones
		 */
		head = page_buffers(page);
		bh = head;
		do {
			BUG_ON(buffer_locked(bh));
			/*
			 * We need to try to allocate
			 * unmapped blocks in the same page.
			 * Otherwise we won't make progress
			 * with the page in ext4_da_writepage
			 */
			if (buffer_dirty(bh) &&
				(!buffer_mapped(bh) || buffer_delay(bh))) {
				mpage_add_bh_to_extent(mpd, logical, bh);
				if (mpd->io_done)
					return MPAGE_DA_EXTENT_TAIL;
			} else if (buffer_dirty(bh) && (buffer_mapped(bh))) {
				/*
				 * mapped dirty buffer. We need to update
				 * the b_state because we look at
				 * b_state in mpage_da_map_blocks. We don't
				 * update b_size because if we find an
				 * unmapped buffer_head later we need to
				 * use the b_state flag of that buffer_head.
				 */
				if (mpd->lbh.b_size == 0)
					mpd->lbh.b_state =
						bh->b_state & BH_FLAGS;
			}
			logical++;
		} while ((bh = bh->b_this_page) != head);
	}

	return 0;
}