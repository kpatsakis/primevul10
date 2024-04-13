static int ext4_index_trans_blocks(struct inode *inode, int nrblocks, int chunk)
{
	if (!(EXT4_I(inode)->i_flags & EXT4_EXTENTS_FL))
		return ext4_indirect_trans_blocks(inode, nrblocks, chunk);
	return ext4_ext_index_trans_blocks(inode, nrblocks, chunk);
}