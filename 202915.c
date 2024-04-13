xfs_ioc_space(
	struct xfs_inode	*ip,
	struct inode		*inode,
	struct file		*filp,
	int			ioflags,
	unsigned int		cmd,
	xfs_flock64_t		*bf)
{
	int			attr_flags = 0;
	int			error;

	/*
	 * Only allow the sys admin to reserve space unless
	 * unwritten extents are enabled.
	 */
	if (!xfs_sb_version_hasextflgbit(&ip->i_mount->m_sb) &&
	    !capable(CAP_SYS_ADMIN))
		return -XFS_ERROR(EPERM);

	if (inode->i_flags & (S_IMMUTABLE|S_APPEND))
		return -XFS_ERROR(EPERM);

	if (!(filp->f_mode & FMODE_WRITE))
		return -XFS_ERROR(EBADF);

	if (!S_ISREG(inode->i_mode))
		return -XFS_ERROR(EINVAL);

	if (filp->f_flags & (O_NDELAY|O_NONBLOCK))
		attr_flags |= XFS_ATTR_NONBLOCK;
	if (ioflags & IO_INVIS)
		attr_flags |= XFS_ATTR_DMI;

	error = xfs_change_file_space(ip, cmd, bf, filp->f_pos, attr_flags);
	return -error;
}