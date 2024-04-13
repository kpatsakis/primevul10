static int __open_namei_create(struct nameidata *nd, struct path *path,
				int flag, int mode)
{
	int error;
	struct dentry *dir = nd->path.dentry;

	if (!IS_POSIXACL(dir->d_inode))
		mode &= ~current->fs->umask;
	error = vfs_create(dir->d_inode, path->dentry, mode, nd);
	mutex_unlock(&dir->d_inode->i_mutex);
	dput(nd->path.dentry);
	nd->path.dentry = path->dentry;
	if (error)
		return error;
	/* Don't check for write permission, don't truncate */
	return may_open(nd, 0, flag & ~O_TRUNC);
}