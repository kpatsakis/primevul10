xfs_inode_free_eofblocks(
	struct xfs_inode	*ip,
	int			flags,
	void			*args)
{
	int ret = 0;
	struct xfs_eofblocks *eofb = args;
	int match;

	if (!xfs_can_free_eofblocks(ip, false)) {
		/* inode could be preallocated or append-only */
		trace_xfs_inode_free_eofblocks_invalid(ip);
		xfs_inode_clear_eofblocks_tag(ip);
		return 0;
	}

	/*
	 * If the mapping is dirty the operation can block and wait for some
	 * time. Unless we are waiting, skip it.
	 */
	if (!(flags & SYNC_WAIT) &&
	    mapping_tagged(VFS_I(ip)->i_mapping, PAGECACHE_TAG_DIRTY))
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

	/*
	 * If the caller is waiting, return -EAGAIN to keep the background
	 * scanner moving and revisit the inode in a subsequent pass.
	 */
	if (!xfs_ilock_nowait(ip, XFS_IOLOCK_EXCL)) {
		if (flags & SYNC_WAIT)
			ret = -EAGAIN;
		return ret;
	}
	ret = xfs_free_eofblocks(ip);
	xfs_iunlock(ip, XFS_IOLOCK_EXCL);

	return ret;
}