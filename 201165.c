static int ext4_calc_metadata_amount(struct inode *inode, int blocks)
{
	if (!blocks)
		return 0;

	if (EXT4_I(inode)->i_flags & EXT4_EXTENTS_FL)
		return ext4_ext_calc_metadata_amount(inode, blocks);

	return ext4_indirect_calc_metadata_amount(inode, blocks);
}