xfs_ioc_fsgetxattr(
	xfs_inode_t		*ip,
	int			attr,
	void			__user *arg)
{
	struct fsxattr		fa;

	memset(&fa, 0, sizeof(struct fsxattr));

	xfs_ilock(ip, XFS_ILOCK_SHARED);
	fa.fsx_xflags = xfs_ip2xflags(ip);
	fa.fsx_extsize = ip->i_d.di_extsize << ip->i_mount->m_sb.sb_blocklog;
	fa.fsx_projid = xfs_get_projid(ip);

	if (attr) {
		if (ip->i_afp) {
			if (ip->i_afp->if_flags & XFS_IFEXTENTS)
				fa.fsx_nextents = ip->i_afp->if_bytes /
							sizeof(xfs_bmbt_rec_t);
			else
				fa.fsx_nextents = ip->i_d.di_anextents;
		} else
			fa.fsx_nextents = 0;
	} else {
		if (ip->i_df.if_flags & XFS_IFEXTENTS)
			fa.fsx_nextents = ip->i_df.if_bytes /
						sizeof(xfs_bmbt_rec_t);
		else
			fa.fsx_nextents = ip->i_d.di_nextents;
	}
	xfs_iunlock(ip, XFS_ILOCK_SHARED);

	if (copy_to_user(arg, &fa, sizeof(fa)))
		return -EFAULT;
	return 0;
}