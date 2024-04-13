xfs_inode_free_cowblocks(
	struct xfs_inode	*ip,
	int			flags,
	void			*args)
{
	struct xfs_eofblocks	*eofb = args;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, XFS_COW_FORK);
	int			match;
	int			ret = 0;

	if (!xfs_prep_free_cowblocks(ip, ifp))
		return 0;

	if (eofb) {
		if (eofb->eof_flags & XFS_EOF_FLAGS_UNION)
			match = xfs_inode_match_id_union(ip, eofb);
		else
			match = xfs_inode_match_id(ip, eofb);
		if (!match)
			return 0;

		/* skip the inode if the file size is too small */
		if (eofb->eof_flags & XFS_EOF_FLAGS_MINFILESIZE &&
		    XFS_ISIZE(ip) < eofb->eof_min_file_size)
			return 0;
	}

	/* Free the CoW blocks */
	xfs_ilock(ip, XFS_IOLOCK_EXCL);
	xfs_ilock(ip, XFS_MMAPLOCK_EXCL);

	/*
	 * Check again, nobody else should be able to dirty blocks or change
	 * the reflink iflag now that we have the first two locks held.
	 */
	if (xfs_prep_free_cowblocks(ip, ifp))
		ret = xfs_reflink_cancel_cow_range(ip, 0, NULLFILEOFF, false);

	xfs_iunlock(ip, XFS_MMAPLOCK_EXCL);
	xfs_iunlock(ip, XFS_IOLOCK_EXCL);

	return ret;
}