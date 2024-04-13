xfs_open_by_handle(
	struct file		*parfilp,
	xfs_fsop_handlereq_t	*hreq)
{
	const struct cred	*cred = current_cred();
	int			error;
	int			fd;
	int			permflag;
	struct file		*filp;
	struct inode		*inode;
	struct dentry		*dentry;

	if (!capable(CAP_SYS_ADMIN))
		return -XFS_ERROR(EPERM);

	dentry = xfs_handlereq_to_dentry(parfilp, hreq);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);
	inode = dentry->d_inode;

	/* Restrict xfs_open_by_handle to directories & regular files. */
	if (!(S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode))) {
		error = -XFS_ERROR(EPERM);
		goto out_dput;
	}

#if BITS_PER_LONG != 32
	hreq->oflags |= O_LARGEFILE;
#endif

	/* Put open permission in namei format. */
	permflag = hreq->oflags;
	if ((permflag+1) & O_ACCMODE)
		permflag++;
	if (permflag & O_TRUNC)
		permflag |= 2;

	if ((!(permflag & O_APPEND) || (permflag & O_TRUNC)) &&
	    (permflag & FMODE_WRITE) && IS_APPEND(inode)) {
		error = -XFS_ERROR(EPERM);
		goto out_dput;
	}

	if ((permflag & FMODE_WRITE) && IS_IMMUTABLE(inode)) {
		error = -XFS_ERROR(EACCES);
		goto out_dput;
	}

	/* Can't write directories. */
	if (S_ISDIR(inode->i_mode) && (permflag & FMODE_WRITE)) {
		error = -XFS_ERROR(EISDIR);
		goto out_dput;
	}

	fd = get_unused_fd();
	if (fd < 0) {
		error = fd;
		goto out_dput;
	}

	filp = dentry_open(dentry, mntget(parfilp->f_path.mnt),
			   hreq->oflags, cred);
	if (IS_ERR(filp)) {
		put_unused_fd(fd);
		return PTR_ERR(filp);
	}

	if (inode->i_mode & S_IFREG) {
		filp->f_flags |= O_NOATIME;
		filp->f_mode |= FMODE_NOCMTIME;
	}

	fd_install(fd, filp);
	return fd;

 out_dput:
	dput(dentry);
	return error;
}