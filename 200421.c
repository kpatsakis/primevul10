int vfs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *new_dentry)
{
	struct inode *inode = old_dentry->d_inode;
	int error;

	if (!inode)
		return -ENOENT;

	error = may_create(dir, new_dentry, NULL);
	if (error)
		return error;

	if (dir->i_sb != inode->i_sb)
		return -EXDEV;

	/*
	 * A link to an append-only or immutable file cannot be created.
	 */
	if (IS_APPEND(inode) || IS_IMMUTABLE(inode))
		return -EPERM;
	if (!dir->i_op || !dir->i_op->link)
		return -EPERM;
	if (S_ISDIR(old_dentry->d_inode->i_mode))
		return -EPERM;

	error = security_inode_link(old_dentry, dir, new_dentry);
	if (error)
		return error;

	mutex_lock(&old_dentry->d_inode->i_mutex);
	DQUOT_INIT(dir);
	error = dir->i_op->link(old_dentry, dir, new_dentry);
	mutex_unlock(&old_dentry->d_inode->i_mutex);
	if (!error)
		fsnotify_link(dir, old_dentry->d_inode, new_dentry);
	return error;
}