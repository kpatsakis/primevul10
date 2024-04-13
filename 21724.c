xfs_inew_wait(
	struct xfs_inode	*ip)
{
	wait_queue_head_t *wq = bit_waitqueue(&ip->i_flags, __XFS_INEW_BIT);
	DEFINE_WAIT_BIT(wait, &ip->i_flags, __XFS_INEW_BIT);

	do {
		prepare_to_wait(wq, &wait.wq_entry, TASK_UNINTERRUPTIBLE);
		if (!xfs_iflags_test(ip, XFS_INEW))
			break;
		schedule();
	} while (true);
	finish_wait(wq, &wait.wq_entry);
}