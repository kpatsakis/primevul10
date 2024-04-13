static void check_preempt_wakeup(struct rq *rq, struct task_struct *p)
{
	struct task_struct *curr = rq->curr;
	struct cfs_rq *cfs_rq = task_cfs_rq(curr);
	struct sched_entity *se = &curr->se, *pse = &p->se;
	unsigned long gran;

	if (unlikely(rt_prio(p->prio))) {
		update_rq_clock(rq);
		update_curr(cfs_rq);
		resched_task(curr);
		return;
	}

	cfs_rq_of(pse)->next = pse;

	/*
	 * Batch tasks do not preempt (their preemption is driven by
	 * the tick):
	 */
	if (unlikely(p->policy == SCHED_BATCH))
		return;

	if (!sched_feat(WAKEUP_PREEMPT))
		return;

	while (!is_same_group(se, pse)) {
		se = parent_entity(se);
		pse = parent_entity(pse);
	}

	gran = sysctl_sched_wakeup_granularity;
	/*
	 * More easily preempt - nice tasks, while not making
	 * it harder for + nice tasks.
	 */
	if (unlikely(se->load.weight > NICE_0_LOAD))
		gran = calc_delta_fair(gran, &se->load);

	if (pse->vruntime + gran < se->vruntime)
		resched_task(curr);
}