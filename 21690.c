xfs_eofblocks_worker(
	struct work_struct *work)
{
	struct xfs_mount *mp = container_of(to_delayed_work(work),
				struct xfs_mount, m_eofblocks_work);
	xfs_icache_free_eofblocks(mp, NULL);
	xfs_queue_eofblocks(mp);
}