asmlinkage long sys_linkat(int olddfd, const char __user *oldname,
			   int newdfd, const char __user *newname,
			   int flags)
{
	struct dentry *new_dentry;
	struct nameidata nd, old_nd;
	int error;
	char * to;

	if ((flags & ~AT_SYMLINK_FOLLOW) != 0)
		return -EINVAL;

	to = getname(newname);
	if (IS_ERR(to))
		return PTR_ERR(to);

	error = __user_walk_fd(olddfd, oldname,
			       flags & AT_SYMLINK_FOLLOW ? LOOKUP_FOLLOW : 0,
			       &old_nd);
	if (error)
		goto exit;
	error = do_path_lookup(newdfd, to, LOOKUP_PARENT, &nd);
	if (error)
		goto out;
	error = -EXDEV;
	if (old_nd.path.mnt != nd.path.mnt)
		goto out_release;
	new_dentry = lookup_create(&nd, 0);
	error = PTR_ERR(new_dentry);
	if (IS_ERR(new_dentry))
		goto out_unlock;
	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto out_dput;
	error = vfs_link(old_nd.path.dentry, nd.path.dentry->d_inode, new_dentry);
	mnt_drop_write(nd.path.mnt);
out_dput:
	dput(new_dentry);
out_unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
out_release:
	path_put(&nd.path);
out:
	path_put(&old_nd.path);
exit:
	putname(to);

	return error;
}