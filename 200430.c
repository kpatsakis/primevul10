asmlinkage long sys_mkdirat(int dfd, const char __user *pathname, int mode)
{
	int error = 0;
	char * tmp;
	struct dentry *dentry;
	struct nameidata nd;

	tmp = getname(pathname);
	error = PTR_ERR(tmp);
	if (IS_ERR(tmp))
		goto out_err;

	error = do_path_lookup(dfd, tmp, LOOKUP_PARENT, &nd);
	if (error)
		goto out;
	dentry = lookup_create(&nd, 1);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_unlock;

	if (!IS_POSIXACL(nd.path.dentry->d_inode))
		mode &= ~current->fs->umask;
	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto out_dput;
	error = vfs_mkdir(nd.path.dentry->d_inode, dentry, mode);
	mnt_drop_write(nd.path.mnt);
out_dput:
	dput(dentry);
out_unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	path_put(&nd.path);
out:
	putname(tmp);
out_err:
	return error;
}