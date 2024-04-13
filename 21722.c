xfs_cowblocks_worker(
	struct work_struct *work)
{
	struct xfs_mount *mp = container_of(to_delayed_work(work),
				struct xfs_mount, m_cowblocks_work);
	xfs_icache_free_cowblocks(mp, NULL);
	xfs_queue_cowblocks(mp);
}