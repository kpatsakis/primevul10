xfs_icache_inode_is_allocated(
	struct xfs_mount	*mp,
	struct xfs_trans	*tp,
	xfs_ino_t		ino,
	bool			*inuse)
{
	struct xfs_inode	*ip;
	int			error;

	error = xfs_iget(mp, tp, ino, XFS_IGET_INCORE, 0, &ip);
	if (error)
		return error;

	*inuse = !!(VFS_I(ip)->i_mode);
	IRELE(ip);
	return 0;
}