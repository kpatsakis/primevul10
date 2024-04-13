xfs_getbmap_format(void **ap, struct getbmapx *bmv, int *full)
{
	struct getbmap __user	*base = *ap;

	/* copy only getbmap portion (not getbmapx) */
	if (copy_to_user(base, bmv, sizeof(struct getbmap)))
		return XFS_ERROR(EFAULT);

	*ap += sizeof(struct getbmap);
	return 0;
}