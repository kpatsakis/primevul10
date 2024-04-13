static void enqueue_sleeper(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
#ifdef CONFIG_SCHEDSTATS
	if (se->sleep_start) {
		u64 delta = rq_of(cfs_rq)->clock - se->sleep_start;
		struct task_struct *tsk = task_of(se);

		if ((s64)delta < 0)
			delta = 0;

		if (unlikely(delta > se->sleep_max))
			se->sleep_max = delta;

		se->sleep_start = 0;
		se->sum_sleep_runtime += delta;

		account_scheduler_latency(tsk, delta >> 10, 1);
	}
	if (se->block_start) {
		u64 delta = rq_of(cfs_rq)->clock - se->block_start;
		struct task_struct *tsk = task_of(se);

		if ((s64)delta < 0)
			delta = 0;

		if (unlikely(delta > se->block_max))
			se->block_max = delta;

		se->block_start = 0;
		se->sum_sleep_runtime += delta;

		/*
		 * Blocking time is in units of nanosecs, so shift by 20 to
		 * get a milliseconds-range estimation of the amount of
		 * time that the task spent sleeping:
		 */
		if (unlikely(prof_on == SLEEP_PROFILING)) {

			profile_hits(SLEEP_PROFILING, (void *)get_wchan(tsk),
				     delta >> 20);
		}
		account_scheduler_latency(tsk, delta >> 10, 0);
	}
#endif
}