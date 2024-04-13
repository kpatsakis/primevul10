xfs_inode_set_eofblocks_tag(
	xfs_inode_t	*ip)
{
	trace_xfs_inode_set_eofblocks_tag(ip);
	return __xfs_inode_set_blocks_tag(ip, xfs_queue_eofblocks,
			trace_xfs_perag_set_eofblocks,
			XFS_ICI_EOFBLOCKS_TAG);
}