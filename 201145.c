static int __ext4_journalled_writepage(struct page *page,
				struct writeback_control *wbc)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	struct buffer_head *page_bufs;
	handle_t *handle = NULL;
	int ret = 0;
	int err;

	ret = block_prepare_write(page, 0, PAGE_CACHE_SIZE,
					ext4_normal_get_block_write);
	if (ret != 0)
		goto out_unlock;

	page_bufs = page_buffers(page);
	walk_page_buffers(handle, page_bufs, 0, PAGE_CACHE_SIZE, NULL,
								bget_one);
	/* As soon as we unlock the page, it can go away, but we have
	 * references to buffers so we are safe */
	unlock_page(page);

	handle = ext4_journal_start(inode, ext4_writepage_trans_blocks(inode));
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		goto out;
	}

	ret = walk_page_buffers(handle, page_bufs, 0,
			PAGE_CACHE_SIZE, NULL, do_journal_get_write_access);

	err = walk_page_buffers(handle, page_bufs, 0,
				PAGE_CACHE_SIZE, NULL, write_end_fn);
	if (ret == 0)
		ret = err;
	err = ext4_journal_stop(handle);
	if (!ret)
		ret = err;

	walk_page_buffers(handle, page_bufs, 0,
				PAGE_CACHE_SIZE, NULL, bput_one);
	EXT4_I(inode)->i_state |= EXT4_STATE_JDATA;
	goto out;

out_unlock:
	unlock_page(page);
out:
	return ret;
}