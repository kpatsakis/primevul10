xfs_queue_eofblocks(
	struct xfs_mount *mp)
{
	rcu_read_lock();
	if (radix_tree_tagged(&mp->m_perag_tree, XFS_ICI_EOFBLOCKS_TAG))
		queue_delayed_work(mp->m_eofblocks_workqueue,
				   &mp->m_eofblocks_work,
				   msecs_to_jiffies(xfs_eofb_secs * 1000));
	rcu_read_unlock();
}