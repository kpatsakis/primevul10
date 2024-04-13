static int ext4_da_reserve_space(struct inode *inode, int nrblocks)
{
	int retries = 0;
       struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
       unsigned long md_needed, mdblocks, total = 0;

	/*
	 * recalculate the amount of metadata blocks to reserve
	 * in order to allocate nrblocks
	 * worse case is one extent per block
	 */
repeat:
	spin_lock(&EXT4_I(inode)->i_block_reservation_lock);
	total = EXT4_I(inode)->i_reserved_data_blocks + nrblocks;
	mdblocks = ext4_calc_metadata_amount(inode, total);
	BUG_ON(mdblocks < EXT4_I(inode)->i_reserved_meta_blocks);

	md_needed = mdblocks - EXT4_I(inode)->i_reserved_meta_blocks;
	total = md_needed + nrblocks;

	if (ext4_claim_free_blocks(sbi, total)) {
		spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);
		if (ext4_should_retry_alloc(inode->i_sb, &retries)) {
			yield();
			goto repeat;
		}
		return -ENOSPC;
	}
	EXT4_I(inode)->i_reserved_data_blocks += nrblocks;
	EXT4_I(inode)->i_reserved_meta_blocks = mdblocks;

	spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);
	return 0;       /* success */
}