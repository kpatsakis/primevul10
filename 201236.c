static int mpage_da_writepages(struct address_space *mapping,
			       struct writeback_control *wbc,
			       struct mpage_da_data *mpd)
{
	int ret;

	if (!mpd->get_block)
		return generic_writepages(mapping, wbc);

	mpd->lbh.b_size = 0;
	mpd->lbh.b_state = 0;
	mpd->lbh.b_blocknr = 0;
	mpd->first_page = 0;
	mpd->next_page = 0;
	mpd->io_done = 0;
	mpd->pages_written = 0;
	mpd->retval = 0;

	ret = write_cache_pages(mapping, wbc, __mpage_da_writepage, mpd);
	/*
	 * Handle last extent of pages
	 */
	if (!mpd->io_done && mpd->next_page != mpd->first_page) {
		if (mpage_da_map_blocks(mpd) == 0)
			mpage_da_submit_io(mpd);

		mpd->io_done = 1;
		ret = MPAGE_DA_EXTENT_TAIL;
	}
	wbc->nr_to_write -= mpd->pages_written;
	return ret;
}