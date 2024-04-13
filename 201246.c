static int ext4_normal_writepage(struct page *page,
				struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	loff_t size = i_size_read(inode);
	loff_t len;

	trace_mark(ext4_normal_writepage,
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

	if (!ext4_journal_current_handle())
		return __ext4_normal_writepage(page, wbc);

	redirty_page_for_writepage(wbc, page);
	unlock_page(page);
	return 0;
}