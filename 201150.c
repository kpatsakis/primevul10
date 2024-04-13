static void ext4_da_update_reserve_space(struct inode *inode, int used)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	int total, mdb, mdb_free;

	spin_lock(&EXT4_I(inode)->i_block_reservation_lock);
	/* recalculate the number of metablocks still need to be reserved */
	total = EXT4_I(inode)->i_reserved_data_blocks - used;
	mdb = ext4_calc_metadata_amount(inode, total);

	/* figure out how many metablocks to release */
	BUG_ON(mdb > EXT4_I(inode)->i_reserved_meta_blocks);
	mdb_free = EXT4_I(inode)->i_reserved_meta_blocks - mdb;

	if (mdb_free) {
		/* Account for allocated meta_blocks */
		mdb_free -= EXT4_I(inode)->i_allocated_meta_blocks;

		/* update fs dirty blocks counter */
		percpu_counter_sub(&sbi->s_dirtyblocks_counter, mdb_free);
		EXT4_I(inode)->i_allocated_meta_blocks = 0;
		EXT4_I(inode)->i_reserved_meta_blocks = mdb;
	}

	/* update per-inode reservations */
	BUG_ON(used  > EXT4_I(inode)->i_reserved_data_blocks);
	EXT4_I(inode)->i_reserved_data_blocks -= used;

	spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);
}