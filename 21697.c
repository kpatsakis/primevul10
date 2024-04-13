xfs_icache_free_eofblocks(
	struct xfs_mount	*mp,
	struct xfs_eofblocks	*eofb)
{
	return __xfs_icache_free_eofblocks(mp, eofb, xfs_inode_free_eofblocks,
			XFS_ICI_EOFBLOCKS_TAG);
}