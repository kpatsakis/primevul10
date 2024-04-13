static int ext4_da_writepages(struct address_space *mapping,
			      struct writeback_control *wbc)
{
	pgoff_t	index;
	int range_whole = 0;
	handle_t *handle = NULL;
	struct mpage_da_data mpd;
	struct inode *inode = mapping->host;
	int no_nrwrite_index_update;
	int pages_written = 0;
	long pages_skipped;
	int needed_blocks, ret = 0, nr_to_writebump = 0;
	struct ext4_sb_info *sbi = EXT4_SB(mapping->host->i_sb);

	trace_mark(ext4_da_writepages,
		   "dev %s ino %lu nr_t_write %ld "
		   "pages_skipped %ld range_start %llu "
		   "range_end %llu nonblocking %d "
		   "for_kupdate %d for_reclaim %d "
		   "for_writepages %d range_cyclic %d",
		   inode->i_sb->s_id, inode->i_ino,
		   wbc->nr_to_write, wbc->pages_skipped,
		   (unsigned long long) wbc->range_start,
		   (unsigned long long) wbc->range_end,
		   wbc->nonblocking, wbc->for_kupdate,
		   wbc->for_reclaim, wbc->for_writepages,
		   wbc->range_cyclic);

	/*
	 * No pages to write? This is mainly a kludge to avoid starting
	 * a transaction for special inodes like journal inode on last iput()
	 * because that could violate lock ordering on umount
	 */
	if (!mapping->nrpages || !mapping_tagged(mapping, PAGECACHE_TAG_DIRTY))
		return 0;

	/*
	 * If the filesystem has aborted, it is read-only, so return
	 * right away instead of dumping stack traces later on that
	 * will obscure the real source of the problem.  We test
	 * EXT4_MOUNT_ABORT instead of sb->s_flag's MS_RDONLY because
	 * the latter could be true if the filesystem is mounted
	 * read-only, and in that case, ext4_da_writepages should
	 * *never* be called, so if that ever happens, we would want
	 * the stack trace.
	 */
	if (unlikely(sbi->s_mount_opt & EXT4_MOUNT_ABORT))
		return -EROFS;

	/*
	 * Make sure nr_to_write is >= sbi->s_mb_stream_request
	 * This make sure small files blocks are allocated in
	 * single attempt. This ensure that small files
	 * get less fragmented.
	 */
	if (wbc->nr_to_write < sbi->s_mb_stream_request) {
		nr_to_writebump = sbi->s_mb_stream_request - wbc->nr_to_write;
		wbc->nr_to_write = sbi->s_mb_stream_request;
	}
	if (wbc->range_start == 0 && wbc->range_end == LLONG_MAX)
		range_whole = 1;

	if (wbc->range_cyclic)
		index = mapping->writeback_index;
	else
		index = wbc->range_start >> PAGE_CACHE_SHIFT;

	mpd.wbc = wbc;
	mpd.inode = mapping->host;

	/*
	 * we don't want write_cache_pages to update
	 * nr_to_write and writeback_index
	 */
	no_nrwrite_index_update = wbc->no_nrwrite_index_update;
	wbc->no_nrwrite_index_update = 1;
	pages_skipped = wbc->pages_skipped;

	while (!ret && wbc->nr_to_write > 0) {

		/*
		 * we  insert one extent at a time. So we need
		 * credit needed for single extent allocation.
		 * journalled mode is currently not supported
		 * by delalloc
		 */
		BUG_ON(ext4_should_journal_data(inode));
		needed_blocks = ext4_da_writepages_trans_blocks(inode);

		/* start a new transaction*/
		handle = ext4_journal_start(inode, needed_blocks);
		if (IS_ERR(handle)) {
			ret = PTR_ERR(handle);
			printk(KERN_CRIT "%s: jbd2_start: "
			       "%ld pages, ino %lu; err %d\n", __func__,
				wbc->nr_to_write, inode->i_ino, ret);
			dump_stack();
			goto out_writepages;
		}
		mpd.get_block = ext4_da_get_block_write;
		ret = mpage_da_writepages(mapping, wbc, &mpd);

		ext4_journal_stop(handle);

		if (mpd.retval == -ENOSPC) {
			/* commit the transaction which would
			 * free blocks released in the transaction
			 * and try again
			 */
			jbd2_journal_force_commit_nested(sbi->s_journal);
			wbc->pages_skipped = pages_skipped;
			ret = 0;
		} else if (ret == MPAGE_DA_EXTENT_TAIL) {
			/*
			 * got one extent now try with
			 * rest of the pages
			 */
			pages_written += mpd.pages_written;
			wbc->pages_skipped = pages_skipped;
			ret = 0;
		} else if (wbc->nr_to_write)
			/*
			 * There is no more writeout needed
			 * or we requested for a noblocking writeout
			 * and we found the device congested
			 */
			break;
	}
	if (pages_skipped != wbc->pages_skipped)
		printk(KERN_EMERG "This should not happen leaving %s "
				"with nr_to_write = %ld ret = %d\n",
				__func__, wbc->nr_to_write, ret);

	/* Update index */
	index += pages_written;
	if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
		/*
		 * set the writeback_index so that range_cyclic
		 * mode will write it back later
		 */
		mapping->writeback_index = index;

out_writepages:
	if (!no_nrwrite_index_update)
		wbc->no_nrwrite_index_update = 0;
	wbc->nr_to_write -= nr_to_writebump;
	trace_mark(ext4_da_writepage_result,
		   "dev %s ino %lu ret %d pages_written %d "
		   "pages_skipped %ld congestion %d "
		   "more_io %d no_nrwrite_index_update %d",
		   inode->i_sb->s_id, inode->i_ino, ret,
		   pages_written, wbc->pages_skipped,
		   wbc->encountered_congestion, wbc->more_io,
		   wbc->no_nrwrite_index_update);
	return ret;
}