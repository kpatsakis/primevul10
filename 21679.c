xfs_icache_free_cowblocks(
	struct xfs_mount	*mp,
	struct xfs_eofblocks	*eofb)
{
	return __xfs_icache_free_eofblocks(mp, eofb, xfs_inode_free_cowblocks,
			XFS_ICI_COWBLOCKS_TAG);
}