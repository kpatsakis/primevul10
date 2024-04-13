xfs_inode_validate_extsize(
	struct xfs_mount		*mp,
	uint32_t			extsize,
	uint16_t			mode,
	uint16_t			flags)
{
	bool				rt_flag;
	bool				hint_flag;
	bool				inherit_flag;
	uint32_t			extsize_bytes;
	uint32_t			blocksize_bytes;

	rt_flag = (flags & XFS_DIFLAG_REALTIME);
	hint_flag = (flags & XFS_DIFLAG_EXTSIZE);
	inherit_flag = (flags & XFS_DIFLAG_EXTSZINHERIT);
	extsize_bytes = XFS_FSB_TO_B(mp, extsize);

	if (rt_flag)
		blocksize_bytes = mp->m_sb.sb_rextsize << mp->m_sb.sb_blocklog;
	else
		blocksize_bytes = mp->m_sb.sb_blocksize;

	if ((hint_flag || inherit_flag) && !(S_ISDIR(mode) || S_ISREG(mode)))
		return __this_address;

	if (hint_flag && !S_ISREG(mode))
		return __this_address;

	if (inherit_flag && !S_ISDIR(mode))
		return __this_address;

	if ((hint_flag || inherit_flag) && extsize == 0)
		return __this_address;

	if (!(hint_flag || inherit_flag) && extsize != 0)
		return __this_address;

	if (extsize_bytes % blocksize_bytes)
		return __this_address;

	if (extsize > MAXEXTLEN)
		return __this_address;

	if (!rt_flag && extsize > mp->m_sb.sb_agblocks / 2)
		return __this_address;

	return NULL;
}