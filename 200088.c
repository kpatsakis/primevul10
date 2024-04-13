static void *shmem_follow_link_inline(struct dentry *dentry, struct nameidata *nd)
{
	nd_set_link(nd, (char *)SHMEM_I(dentry->d_inode));
	return NULL;
}