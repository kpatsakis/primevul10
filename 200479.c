int permission(struct inode *inode, int mask, struct nameidata *nd)
{
	int retval, submask;
	struct vfsmount *mnt = NULL;

	if (nd)
		mnt = nd->path.mnt;

	if (mask & MAY_WRITE) {
		umode_t mode = inode->i_mode;

		/*
		 * Nobody gets write access to a read-only fs.
		 */
		if (IS_RDONLY(inode) &&
		    (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)))
			return -EROFS;

		/*
		 * Nobody gets write access to an immutable file.
		 */
		if (IS_IMMUTABLE(inode))
			return -EACCES;
	}

	if ((mask & MAY_EXEC) && S_ISREG(inode->i_mode)) {
		/*
		 * MAY_EXEC on regular files is denied if the fs is mounted
		 * with the "noexec" flag.
		 */
		if (mnt && (mnt->mnt_flags & MNT_NOEXEC))
			return -EACCES;
	}

	/* Ordinary permission routines do not understand MAY_APPEND. */
	submask = mask & ~MAY_APPEND;
	if (inode->i_op && inode->i_op->permission) {
		retval = inode->i_op->permission(inode, submask, nd);
		if (!retval) {
			/*
			 * Exec permission on a regular file is denied if none
			 * of the execute bits are set.
			 *
			 * This check should be done by the ->permission()
			 * method.
			 */
			if ((mask & MAY_EXEC) && S_ISREG(inode->i_mode) &&
			    !(inode->i_mode & S_IXUGO))
				return -EACCES;
		}
	} else {
		retval = generic_permission(inode, submask, NULL);
	}
	if (retval)
		return retval;

	retval = devcgroup_inode_permission(inode, mask);
	if (retval)
		return retval;

	return security_inode_permission(inode, mask, nd);
}