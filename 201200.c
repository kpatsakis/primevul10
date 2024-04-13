static int  mpage_da_map_blocks(struct mpage_da_data *mpd)
{
	int err = 0;
	struct buffer_head new;
	struct buffer_head *lbh = &mpd->lbh;
	sector_t next;

	/*
	 * We consider only non-mapped and non-allocated blocks
	 */
	if (buffer_mapped(lbh) && !buffer_delay(lbh))
		return 0;
	new.b_state = lbh->b_state;
	new.b_blocknr = 0;
	new.b_size = lbh->b_size;
	next = lbh->b_blocknr;
	/*
	 * If we didn't accumulate anything
	 * to write simply return
	 */
	if (!new.b_size)
		return 0;
	err = mpd->get_block(mpd->inode, next, &new, 1);
	if (err) {

		/* If get block returns with error
		 * we simply return. Later writepage
		 * will redirty the page and writepages
		 * will find the dirty page again
		 */
		if (err == -EAGAIN)
			return 0;

		if (err == -ENOSPC &&
				ext4_count_free_blocks(mpd->inode->i_sb)) {
			mpd->retval = err;
			return 0;
		}

		/*
		 * get block failure will cause us
		 * to loop in writepages. Because
		 * a_ops->writepage won't be able to
		 * make progress. The page will be redirtied
		 * by writepage and writepages will again
		 * try to write the same.
		 */
		printk(KERN_EMERG "%s block allocation failed for inode %lu "
				  "at logical offset %llu with max blocks "
				  "%zd with error %d\n",
				  __func__, mpd->inode->i_ino,
				  (unsigned long long)next,
				  lbh->b_size >> mpd->inode->i_blkbits, err);
		printk(KERN_EMERG "This should not happen.!! "
					"Data will be lost\n");
		if (err == -ENOSPC) {
			ext4_print_free_blocks(mpd->inode);
		}
		/* invlaidate all the pages */
		ext4_da_block_invalidatepages(mpd, next,
				lbh->b_size >> mpd->inode->i_blkbits);
		return err;
	}
	BUG_ON(new.b_size == 0);

	if (buffer_new(&new))
		__unmap_underlying_blocks(mpd->inode, &new);

	/*
	 * If blocks are delayed marked, we need to
	 * put actual blocknr and drop delayed bit
	 */
	if (buffer_delay(lbh) || buffer_unwritten(lbh))
		mpage_put_bnr_to_bhs(mpd, next, &new);

	return 0;
}