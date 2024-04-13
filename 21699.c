xfs_reclaim_worker(
	struct work_struct *work)
{
	struct xfs_mount *mp = container_of(to_delayed_work(work),
					struct xfs_mount, m_reclaim_work);

	xfs_reclaim_inodes(mp, SYNC_TRYLOCK);
	xfs_reclaim_work_queue(mp);
}