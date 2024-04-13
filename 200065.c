static void shmem_truncate(struct inode *inode)
{
	shmem_truncate_range(inode, inode->i_size, (loff_t)-1);
}