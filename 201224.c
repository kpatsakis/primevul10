static void mpage_add_bh_to_extent(struct mpage_da_data *mpd,
				   sector_t logical, struct buffer_head *bh)
{
	sector_t next;
	size_t b_size = bh->b_size;
	struct buffer_head *lbh = &mpd->lbh;
	int nrblocks = lbh->b_size >> mpd->inode->i_blkbits;

	/* check if thereserved journal credits might overflow */
	if (!(EXT4_I(mpd->inode)->i_flags & EXT4_EXTENTS_FL)) {
		if (nrblocks >= EXT4_MAX_TRANS_DATA) {
			/*
			 * With non-extent format we are limited by the journal
			 * credit available.  Total credit needed to insert
			 * nrblocks contiguous blocks is dependent on the
			 * nrblocks.  So limit nrblocks.
			 */
			goto flush_it;
		} else if ((nrblocks + (b_size >> mpd->inode->i_blkbits)) >
				EXT4_MAX_TRANS_DATA) {
			/*
			 * Adding the new buffer_head would make it cross the
			 * allowed limit for which we have journal credit
			 * reserved. So limit the new bh->b_size
			 */
			b_size = (EXT4_MAX_TRANS_DATA - nrblocks) <<
						mpd->inode->i_blkbits;
			/* we will do mpage_da_submit_io in the next loop */
		}
	}
	/*
	 * First block in the extent
	 */
	if (lbh->b_size == 0) {
		lbh->b_blocknr = logical;
		lbh->b_size = b_size;
		lbh->b_state = bh->b_state & BH_FLAGS;
		return;
	}

	next = lbh->b_blocknr + nrblocks;
	/*
	 * Can we merge the block to our big extent?
	 */
	if (logical == next && (bh->b_state & BH_FLAGS) == lbh->b_state) {
		lbh->b_size += b_size;
		return;
	}

flush_it:
	/*
	 * We couldn't merge the block to our extent, so we
	 * need to flush current  extent and start new one
	 */
	if (mpage_da_map_blocks(mpd) == 0)
		mpage_da_submit_io(mpd);
	mpd->io_done = 1;
	return;
}