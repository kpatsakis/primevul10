static void shmem_free_blocks(struct inode *inode, long pages)
{
	struct shmem_sb_info *sbinfo = SHMEM_SB(inode->i_sb);
	if (sbinfo->max_blocks) {
		spin_lock(&sbinfo->stat_lock);
		sbinfo->free_blocks += pages;
		inode->i_blocks -= pages*BLOCKS_PER_PAGE;
		spin_unlock(&sbinfo->stat_lock);
	}
}