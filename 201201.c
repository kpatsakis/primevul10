static void ext4_da_release_space(struct inode *inode, int to_free)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	int total, mdb, mdb_free, release;

	if (!to_free)
		return;		/* Nothing to release, exit */

	spin_lock(&EXT4_I(inode)->i_block_reservation_lock);

	if (!EXT4_I(inode)->i_reserved_data_blocks) {
		/*
		 * if there is no reserved blocks, but we try to free some
		 * then the counter is messed up somewhere.
		 * but since this function is called from invalidate
		 * page, it's harmless to return without any action
		 */
		printk(KERN_INFO "ext4 delalloc try to release %d reserved "
			    "blocks for inode %lu, but there is no reserved "
			    "data blocks\n", to_free, inode->i_ino);
		spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);
		return;
	}

	/* recalculate the number of metablocks still need to be reserved */
	total = EXT4_I(inode)->i_reserved_data_blocks - to_free;
	mdb = ext4_calc_metadata_amount(inode, total);

	/* figure out how many metablocks to release */
	BUG_ON(mdb > EXT4_I(inode)->i_reserved_meta_blocks);
	mdb_free = EXT4_I(inode)->i_reserved_meta_blocks - mdb;

	release = to_free + mdb_free;

	/* update fs dirty blocks counter for truncate case */
	percpu_counter_sub(&sbi->s_dirtyblocks_counter, release);

	/* update per-inode reservations */
	BUG_ON(to_free > EXT4_I(inode)->i_reserved_data_blocks);
	EXT4_I(inode)->i_reserved_data_blocks -= to_free;

	BUG_ON(mdb > EXT4_I(inode)->i_reserved_meta_blocks);
	EXT4_I(inode)->i_reserved_meta_blocks = mdb;
	spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);
}