xfs_getbmapx_format(void **ap, struct getbmapx *bmv, int *full)
{
	struct getbmapx __user	*base = *ap;

	if (copy_to_user(base, bmv, sizeof(struct getbmapx)))
		return XFS_ERROR(EFAULT);

	*ap += sizeof(struct getbmapx);
	return 0;
}