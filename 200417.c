int may_open(struct nameidata *nd, int acc_mode, int flag)
{
	struct dentry *dentry = nd->path.dentry;
	struct inode *inode = dentry->d_inode;
	int error;

	if (!inode)
		return -ENOENT;

	if (S_ISLNK(inode->i_mode))
		return -ELOOP;
	
	if (S_ISDIR(inode->i_mode) && (acc_mode & MAY_WRITE))
		return -EISDIR;

	/*
	 * FIFO's, sockets and device files are special: they don't
	 * actually live on the filesystem itself, and as such you
	 * can write to them even if the filesystem is read-only.
	 */
	if (S_ISFIFO(inode->i_mode) || S_ISSOCK(inode->i_mode)) {
	    	flag &= ~O_TRUNC;
	} else if (S_ISBLK(inode->i_mode) || S_ISCHR(inode->i_mode)) {
		if (nd->path.mnt->mnt_flags & MNT_NODEV)
			return -EACCES;

		flag &= ~O_TRUNC;
	}

	error = vfs_permission(nd, acc_mode);
	if (error)
		return error;
	/*
	 * An append-only file must be opened in append mode for writing.
	 */
	if (IS_APPEND(inode)) {
		if  ((flag & FMODE_WRITE) && !(flag & O_APPEND))
			return -EPERM;
		if (flag & O_TRUNC)
			return -EPERM;
	}

	/* O_NOATIME can only be set by the owner or superuser */
	if (flag & O_NOATIME)
		if (!is_owner_or_cap(inode))
			return -EPERM;

	/*
	 * Ensure there are no outstanding leases on the file.
	 */
	error = break_lease(inode, flag);
	if (error)
		return error;

	if (flag & O_TRUNC) {
		error = get_write_access(inode);
		if (error)
			return error;

		/*
		 * Refuse to truncate files with mandatory locks held on them.
		 */
		error = locks_verify_locked(inode);
		if (!error) {
			DQUOT_INIT(inode);

			error = do_truncate(dentry, 0,
					    ATTR_MTIME|ATTR_CTIME|ATTR_OPEN,
					    NULL);
		}
		put_write_access(inode);
		if (error)
			return error;
	} else
		if (flag & FMODE_WRITE)
			DQUOT_INIT(inode);

	return 0;
}