nfs4_ff_layout_stat_io_update_completed(struct nfs4_ff_layoutstat *layoutstat,
		__u64 requested,
		__u64 completed,
		ktime_t time_completed,
		ktime_t time_started)
{
	struct nfs4_ff_io_stat *iostat = &layoutstat->io_stat;
	ktime_t completion_time = ktime_sub(time_completed, time_started);
	ktime_t timer;

	iostat->ops_completed++;
	iostat->bytes_completed += completed;
	iostat->bytes_not_delivered += requested - completed;

	timer = nfs4_ff_end_busy_timer(&layoutstat->busy_timer, time_completed);
	iostat->total_busy_time =
			ktime_add(iostat->total_busy_time, timer);
	iostat->aggregate_completion_time =
			ktime_add(iostat->aggregate_completion_time,
					completion_time);
}