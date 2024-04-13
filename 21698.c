xfs_inode_match_id(
	struct xfs_inode	*ip,
	struct xfs_eofblocks	*eofb)
{
	if ((eofb->eof_flags & XFS_EOF_FLAGS_UID) &&
	    !uid_eq(VFS_I(ip)->i_uid, eofb->eof_uid))
		return 0;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_GID) &&
	    !gid_eq(VFS_I(ip)->i_gid, eofb->eof_gid))
		return 0;

	if ((eofb->eof_flags & XFS_EOF_FLAGS_PRID) &&
	    xfs_get_projid(ip) != eofb->eof_prid)
		return 0;

	return 1;
}