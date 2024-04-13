static int cfs_rq_best_prio(struct cfs_rq *cfs_rq)
{
	struct sched_entity *curr;
	struct task_struct *p;

	if (!cfs_rq->nr_running || !first_fair(cfs_rq))
		return MAX_PRIO;

	curr = cfs_rq->curr;
	if (!curr)
		curr = __pick_next_entity(cfs_rq);

	p = task_of(curr);

	return p->prio;
}