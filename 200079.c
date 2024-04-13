static int shmem_create(struct inode *dir, struct dentry *dentry, int mode,
		struct nameidata *nd)
{
	return shmem_mknod(dir, dentry, mode | S_IFREG, 0);
}