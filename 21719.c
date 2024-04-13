xfs_iflag_for_tag(
	int		tag)
{
	switch (tag) {
	case XFS_ICI_EOFBLOCKS_TAG:
		return XFS_IEOFBLOCKS;
	case XFS_ICI_COWBLOCKS_TAG:
		return XFS_ICOWBLOCKS;
	default:
		ASSERT(0);
		return 0;
	}
}