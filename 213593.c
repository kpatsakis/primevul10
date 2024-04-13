nfs4_ff_layoutstat_start_io(struct nfs4_ff_layout_mirror *mirror,
			    struct nfs4_ff_layoutstat *layoutstat,
			    ktime_t now)
{
	s64 report_interval = FF_LAYOUTSTATS_REPORT_INTERVAL;
	struct nfs4_flexfile_layout *ffl = FF_LAYOUT_FROM_HDR(mirror->layout);

	nfs4_ff_start_busy_timer(&layoutstat->busy_timer, now);
	if (!mirror->start_time)
		mirror->start_time = now;
	if (mirror->report_interval != 0)
		report_interval = (s64)mirror->report_interval * 1000LL;
	else if (layoutstats_timer != 0)
		report_interval = (s64)layoutstats_timer * 1000LL;
	if (ktime_to_ms(ktime_sub(now, ffl->last_report_time)) >=
			report_interval) {
		ffl->last_report_time = now;
		return true;
	}

	return false;
}