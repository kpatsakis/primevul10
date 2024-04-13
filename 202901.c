xfs_ioc_getbmap(
	struct xfs_inode	*ip,
	int			ioflags,
	unsigned int		cmd,
	void			__user *arg)
{
	struct getbmapx		bmx;
	int			error;

	if (copy_from_user(&bmx, arg, sizeof(struct getbmapx)))
		return -XFS_ERROR(EFAULT);

	if (bmx.bmv_count < 2)
		return -XFS_ERROR(EINVAL);

	bmx.bmv_iflags = (cmd == XFS_IOC_GETBMAPA ? BMV_IF_ATTRFORK : 0);
	if (ioflags & IO_INVIS)
		bmx.bmv_iflags |= BMV_IF_NO_DMAPI_READ;

	error = xfs_getbmap(ip, &bmx, xfs_getbmap_format,
			    (struct getbmap *)arg+1);
	if (error)
		return -error;

	/* copy back header - only size of getbmap */
	if (copy_to_user(arg, &bmx, sizeof(struct getbmap)))
		return -XFS_ERROR(EFAULT);
	return 0;
}