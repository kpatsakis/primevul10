xfs_ioc_getbmapx(
	struct xfs_inode	*ip,
	void			__user *arg)
{
	struct getbmapx		bmx;
	int			error;

	if (copy_from_user(&bmx, arg, sizeof(bmx)))
		return -XFS_ERROR(EFAULT);

	if (bmx.bmv_count < 2)
		return -XFS_ERROR(EINVAL);

	if (bmx.bmv_iflags & (~BMV_IF_VALID))
		return -XFS_ERROR(EINVAL);

	error = xfs_getbmap(ip, &bmx, xfs_getbmapx_format,
			    (struct getbmapx *)arg+1);
	if (error)
		return -error;

	/* copy back header */
	if (copy_to_user(arg, &bmx, sizeof(struct getbmapx)))
		return -XFS_ERROR(EFAULT);

	return 0;
}