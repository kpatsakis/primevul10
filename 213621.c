nfs4_ff_layout_stat_io_start_read(struct inode *inode,
		struct nfs4_ff_layout_mirror *mirror,
		__u64 requested, ktime_t now)
{
	bool report;

	spin_lock(&mirror->lock);
	report = nfs4_ff_layoutstat_start_io(mirror, &mirror->read_stat, now);
	nfs4_ff_layout_stat_io_update_requested(&mirror->read_stat, requested);
	set_bit(NFS4_FF_MIRROR_STAT_AVAIL, &mirror->flags);
	spin_unlock(&mirror->lock);

	if (report)
		pnfs_report_layoutstat(inode, GFP_KERNEL);
}