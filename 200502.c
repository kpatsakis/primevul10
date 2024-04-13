int vfs_permission(struct nameidata *nd, int mask)
{
	return permission(nd->path.dentry->d_inode, mask, nd);
}