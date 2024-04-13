static int ext4_indirect_calc_metadata_amount(struct inode *inode, int blocks)
{
	int icap = EXT4_ADDR_PER_BLOCK(inode->i_sb);
	int ind_blks, dind_blks, tind_blks;

	/* number of new indirect blocks needed */
	ind_blks = (blocks + icap - 1) / icap;

	dind_blks = (ind_blks + icap - 1) / icap;

	tind_blks = 1;

	return ind_blks + dind_blks + tind_blks;
}