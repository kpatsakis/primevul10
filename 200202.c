static u64 sched_vslice(struct cfs_rq *cfs_rq)
{
	return __sched_vslice(cfs_rq->load.weight, cfs_rq->nr_running);
}