nfs4_ff_layout_stat_io_end_read(struct rpc_task *task,
		struct nfs4_ff_layout_mirror *mirror,
		__u64 requested,
		__u64 completed)
{
	spin_lock(&mirror->lock);
	nfs4_ff_layout_stat_io_update_completed(&mirror->read_stat,
			requested, completed,
			ktime_get(), task->tk_start);
	set_bit(NFS4_FF_MIRROR_STAT_AVAIL, &mirror->flags);
	spin_unlock(&mirror->lock);
}