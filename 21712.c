xfs_inode_clear_cowblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_clear_cowblocks_tag(ip);
	return __xfs_inode_clear_blocks_tag(ip,
			trace_xfs_perag_clear_cowblocks, XFS_ICI_COWBLOCKS_TAG);
}