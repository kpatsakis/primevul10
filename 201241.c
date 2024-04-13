static void ext4_da_block_invalidatepages(struct mpage_da_data *mpd,
					sector_t logical, long blk_cnt)
{
	int nr_pages, i;
	pgoff_t index, end;
	struct pagevec pvec;
	struct inode *inode = mpd->inode;
	struct address_space *mapping = inode->i_mapping;

	index = logical >> (PAGE_CACHE_SHIFT - inode->i_blkbits);
	end   = (logical + blk_cnt - 1) >>
				(PAGE_CACHE_SHIFT - inode->i_blkbits);
	while (index <= end) {
		nr_pages = pagevec_lookup(&pvec, mapping, index, PAGEVEC_SIZE);
		if (nr_pages == 0)
			break;
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
			index = page->index;
			if (index > end)
				break;
			index++;

			BUG_ON(!PageLocked(page));
			BUG_ON(PageWriteback(page));
			block_invalidatepage(page, 0);
			ClearPageUptodate(page);
			unlock_page(page);
		}
	}
	return;
}