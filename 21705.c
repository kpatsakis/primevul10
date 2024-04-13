xfs_inode_set_cowblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_set_cowblocks_tag(ip);
	return __xfs_inode_set_blocks_tag(ip, xfs_queue_cowblocks,
			trace_xfs_perag_set_cowblocks,
			XFS_ICI_COWBLOCKS_TAG);
}