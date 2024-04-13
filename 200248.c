pick_next(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
	s64 diff, gran;

	if (!cfs_rq->next)
		return se;

	diff = cfs_rq->next->vruntime - se->vruntime;
	if (diff < 0)
		return se;

	gran = calc_delta_fair(sysctl_sched_wakeup_granularity, &cfs_rq->load);
	if (diff > gran)
		return se;

	return cfs_rq->next;
}