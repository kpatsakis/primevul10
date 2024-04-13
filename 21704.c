xfs_inode_clear_eofblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_clear_eofblocks_tag(ip);
	return __xfs_inode_clear_blocks_tag(ip,
			trace_xfs_perag_clear_eofblocks, XFS_ICI_EOFBLOCKS_TAG);
}