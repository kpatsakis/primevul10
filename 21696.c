__xfs_icache_free_eofblocks(
	struct xfs_mount	*mp,
	struct xfs_eofblocks	*eofb,
	int			(*execute)(struct xfs_inode *ip, int flags,
					   void *args),
	int			tag)
{
	int flags = SYNC_TRYLOCK;

	if (eofb && (eofb->eof_flags & XFS_EOF_FLAGS_SYNC))
		flags = SYNC_WAIT;

	return xfs_inode_ag_iterator_tag(mp, execute, flags,
					 eofb, tag);
}