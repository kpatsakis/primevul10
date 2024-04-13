static int wake_idle(int cpu, struct task_struct *p)
{
	cpumask_t tmp;
	struct sched_domain *sd;
	int i;

	/*
	 * If it is idle, then it is the best cpu to run this task.
	 *
	 * This cpu is also the best, if it has more than one task already.
	 * Siblings must be also busy(in most cases) as they didn't already
	 * pickup the extra load from this cpu and hence we need not check
	 * sibling runqueue info. This will avoid the checks and cache miss
	 * penalities associated with that.
	 */
	if (idle_cpu(cpu) || cpu_rq(cpu)->nr_running > 1)
		return cpu;

	for_each_domain(cpu, sd) {
		if (sd->flags & SD_WAKE_IDLE) {
			cpus_and(tmp, sd->span, p->cpus_allowed);
			for_each_cpu_mask(i, tmp) {
				if (idle_cpu(i)) {
					if (i != task_cpu(p)) {
						schedstat_inc(p,
						       se.nr_wakeups_idle);
					}
					return i;
				}
			}
		} else {
			break;
		}
	}
	return cpu;
}