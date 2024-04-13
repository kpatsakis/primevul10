xfs_inode_free_quota_cowblocks(
	struct xfs_inode *ip)
{
	return __xfs_inode_free_quota_eofblocks(ip, xfs_icache_free_cowblocks);
}