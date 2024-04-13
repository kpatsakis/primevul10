static int ext4_indirect_trans_blocks(struct inode *inode, int nrblocks,
				      int chunk)
{
	int indirects;

	/* if nrblocks are contiguous */
	if (chunk) {
		/*
		 * With N contiguous data blocks, it need at most
		 * N/EXT4_ADDR_PER_BLOCK(inode->i_sb) indirect blocks
		 * 2 dindirect blocks
		 * 1 tindirect block
		 */
		indirects = nrblocks / EXT4_ADDR_PER_BLOCK(inode->i_sb);
		return indirects + 3;
	}
	/*
	 * if nrblocks are not contiguous, worse case, each block touch
	 * a indirect block, and each indirect block touch a double indirect
	 * block, plus a triple indirect block
	 */
	indirects = nrblocks * 2 + 1;
	return indirects;
}