int ext4_getattr(struct vfsmount *mnt, struct dentry *dentry,
		 struct kstat *stat)
{
	struct inode *inode;
	unsigned long delalloc_blocks;

	inode = dentry->d_inode;
	generic_fillattr(inode, stat);

	/*
	 * We can't update i_blocks if the block allocation is delayed
	 * otherwise in the case of system crash before the real block
	 * allocation is done, we will have i_blocks inconsistent with
	 * on-disk file blocks.
	 * We always keep i_blocks updated together with real
	 * allocation. But to not confuse with user, stat
	 * will return the blocks that include the delayed allocation
	 * blocks for this file.
	 */
	spin_lock(&EXT4_I(inode)->i_block_reservation_lock);
	delalloc_blocks = EXT4_I(inode)->i_reserved_data_blocks;
	spin_unlock(&EXT4_I(inode)->i_block_reservation_lock);

	stat->blocks += (delalloc_blocks << inode->i_sb->s_blocksize_bits)>>9;
	return 0;
}