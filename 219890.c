xfs_dinode_good_version(
	struct xfs_mount *mp,
	__u8		version)
{
	if (xfs_sb_version_hascrc(&mp->m_sb))
		return version == 3;

	return version == 1 || version == 2;
}