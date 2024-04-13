int ext4_get_blocks_wrap(handle_t *handle, struct inode *inode, sector_t block,
			unsigned int max_blocks, struct buffer_head *bh,
			int create, int extend_disksize, int flag)
{
	int retval;

	clear_buffer_mapped(bh);

	/*
	 * Try to see if we can get  the block without requesting
	 * for new file system block.
	 */
	down_read((&EXT4_I(inode)->i_data_sem));
	if (EXT4_I(inode)->i_flags & EXT4_EXTENTS_FL) {
		retval =  ext4_ext_get_blocks(handle, inode, block, max_blocks,
				bh, 0, 0);
	} else {
		retval = ext4_get_blocks_handle(handle,
				inode, block, max_blocks, bh, 0, 0);
	}
	up_read((&EXT4_I(inode)->i_data_sem));

	/* If it is only a block(s) look up */
	if (!create)
		return retval;

	/*
	 * Returns if the blocks have already allocated
	 *
	 * Note that if blocks have been preallocated
	 * ext4_ext_get_block() returns th create = 0
	 * with buffer head unmapped.
	 */
	if (retval > 0 && buffer_mapped(bh))
		return retval;

	/*
	 * New blocks allocate and/or writing to uninitialized extent
	 * will possibly result in updating i_data, so we take
	 * the write lock of i_data_sem, and call get_blocks()
	 * with create == 1 flag.
	 */
	down_write((&EXT4_I(inode)->i_data_sem));

	/*
	 * if the caller is from delayed allocation writeout path
	 * we have already reserved fs blocks for allocation
	 * let the underlying get_block() function know to
	 * avoid double accounting
	 */
	if (flag)
		EXT4_I(inode)->i_delalloc_reserved_flag = 1;
	/*
	 * We need to check for EXT4 here because migrate
	 * could have changed the inode type in between
	 */
	if (EXT4_I(inode)->i_flags & EXT4_EXTENTS_FL) {
		retval =  ext4_ext_get_blocks(handle, inode, block, max_blocks,
				bh, create, extend_disksize);
	} else {
		retval = ext4_get_blocks_handle(handle, inode, block,
				max_blocks, bh, create, extend_disksize);

		if (retval > 0 && buffer_new(bh)) {
			/*
			 * We allocated new blocks which will result in
			 * i_data's format changing.  Force the migrate
			 * to fail by clearing migrate flags
			 */
			EXT4_I(inode)->i_flags = EXT4_I(inode)->i_flags &
							~EXT4_EXT_MIGRATE;
		}
	}

	if (flag) {
		EXT4_I(inode)->i_delalloc_reserved_flag = 0;
		/*
		 * Update reserved blocks/metadata blocks
		 * after successful block allocation
		 * which were deferred till now
		 */
		if ((retval > 0) && buffer_delay(bh))
			ext4_da_update_reserve_space(inode, retval);
	}

	up_write((&EXT4_I(inode)->i_data_sem));
	return retval;
}