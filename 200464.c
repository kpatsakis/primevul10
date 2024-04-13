asmlinkage long sys_symlinkat(const char __user *oldname,
			      int newdfd, const char __user *newname)
{
	int error = 0;
	char * from;
	char * to;
	struct dentry *dentry;
	struct nameidata nd;

	from = getname(oldname);
	if(IS_ERR(from))
		return PTR_ERR(from);
	to = getname(newname);
	error = PTR_ERR(to);
	if (IS_ERR(to))
		goto out_putname;

	error = do_path_lookup(newdfd, to, LOOKUP_PARENT, &nd);
	if (error)
		goto out;
	dentry = lookup_create(&nd, 0);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_unlock;

	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto out_dput;
	error = vfs_symlink(nd.path.dentry->d_inode, dentry, from, S_IALLUGO);
	mnt_drop_write(nd.path.mnt);
out_dput:
	dput(dentry);
out_unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	path_put(&nd.path);
out:
	putname(to);
out_putname:
	putname(from);
	return error;
}