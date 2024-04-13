__xfs_inode_free_quota_eofblocks(
	struct xfs_inode	*ip,
	int			(*execute)(struct xfs_mount *mp,
					   struct xfs_eofblocks	*eofb))
{
	int scan = 0;
	struct xfs_eofblocks eofb = {0};
	struct xfs_dquot *dq;

	/*
	 * Run a sync scan to increase effectiveness and use the union filter to
	 * cover all applicable quotas in a single scan.
	 */
	eofb.eof_flags = XFS_EOF_FLAGS_UNION|XFS_EOF_FLAGS_SYNC;

	if (XFS_IS_UQUOTA_ENFORCED(ip->i_mount)) {
		dq = xfs_inode_dquot(ip, XFS_DQ_USER);
		if (dq && xfs_dquot_lowsp(dq)) {
			eofb.eof_uid = VFS_I(ip)->i_uid;
			eofb.eof_flags |= XFS_EOF_FLAGS_UID;
			scan = 1;
		}
	}

	if (XFS_IS_GQUOTA_ENFORCED(ip->i_mount)) {
		dq = xfs_inode_dquot(ip, XFS_DQ_GROUP);
		if (dq && xfs_dquot_lowsp(dq)) {
			eofb.eof_gid = VFS_I(ip)->i_gid;
			eofb.eof_flags |= XFS_EOF_FLAGS_GID;
			scan = 1;
		}
	}

	if (scan)
		execute(ip->i_mount, &eofb);

	return scan;
}