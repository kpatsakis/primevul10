static int ext4_da_get_block_write(struct inode *inode, sector_t iblock,
				   struct buffer_head *bh_result, int create)
{
	int ret;
	unsigned max_blocks = bh_result->b_size >> inode->i_blkbits;
	loff_t disksize = EXT4_I(inode)->i_disksize;
	handle_t *handle = NULL;

	handle = ext4_journal_current_handle();
	BUG_ON(!handle);
	ret = ext4_get_blocks_wrap(handle, inode, iblock, max_blocks,
			bh_result, create, 0, EXT4_DELALLOC_RSVED);
	if (ret > 0) {

		bh_result->b_size = (ret << inode->i_blkbits);

		if (ext4_should_order_data(inode)) {
			int retval;
			retval = ext4_jbd2_file_inode(handle, inode);
			if (retval)
				/*
				 * Failed to add inode for ordered
				 * mode. Don't update file size
				 */
				return retval;
		}

		/*
		 * Update on-disk size along with block allocation
		 * we don't use 'extend_disksize' as size may change
		 * within already allocated block -bzzz
		 */
		disksize = ((loff_t) iblock + ret) << inode->i_blkbits;
		if (disksize > i_size_read(inode))
			disksize = i_size_read(inode);
		if (disksize > EXT4_I(inode)->i_disksize) {
			ext4_update_i_disksize(inode, disksize);
			ret = ext4_mark_inode_dirty(handle, inode);
			return ret;
		}
		ret = 0;
	}
	return ret;
}