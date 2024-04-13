static int select_task_rq_fair(struct task_struct *p, int sync)
{
	int cpu, this_cpu;
	struct rq *rq;
	struct sched_domain *sd, *this_sd = NULL;
	int new_cpu;

	cpu      = task_cpu(p);
	rq       = task_rq(p);
	this_cpu = smp_processor_id();
	new_cpu  = cpu;

	if (cpu == this_cpu)
		goto out_set_cpu;

	for_each_domain(this_cpu, sd) {
		if (cpu_isset(cpu, sd->span)) {
			this_sd = sd;
			break;
		}
	}

	if (unlikely(!cpu_isset(this_cpu, p->cpus_allowed)))
		goto out_set_cpu;

	/*
	 * Check for affine wakeup and passive balancing possibilities.
	 */
	if (this_sd) {
		int idx = this_sd->wake_idx;
		unsigned int imbalance;
		unsigned long load, this_load;

		imbalance = 100 + (this_sd->imbalance_pct - 100) / 2;

		load = source_load(cpu, idx);
		this_load = target_load(this_cpu, idx);

		new_cpu = this_cpu; /* Wake to this CPU if we can */

		if (this_sd->flags & SD_WAKE_AFFINE) {
			unsigned long tl = this_load;
			unsigned long tl_per_task;

			/*
			 * Attract cache-cold tasks on sync wakeups:
			 */
			if (sync && !task_hot(p, rq->clock, this_sd))
				goto out_set_cpu;

			schedstat_inc(p, se.nr_wakeups_affine_attempts);
			tl_per_task = cpu_avg_load_per_task(this_cpu);

			/*
			 * If sync wakeup then subtract the (maximum possible)
			 * effect of the currently running task from the load
			 * of the current CPU:
			 */
			if (sync)
				tl -= current->se.load.weight;

			if ((tl <= load &&
				tl + target_load(cpu, idx) <= tl_per_task) ||
			       100*(tl + p->se.load.weight) <= imbalance*load) {
				/*
				 * This domain has SD_WAKE_AFFINE and
				 * p is cache cold in this domain, and
				 * there is no bad imbalance.
				 */
				schedstat_inc(this_sd, ttwu_move_affine);
				schedstat_inc(p, se.nr_wakeups_affine);
				goto out_set_cpu;
			}
		}

		/*
		 * Start passive balancing when half the imbalance_pct
		 * limit is reached.
		 */
		if (this_sd->flags & SD_WAKE_BALANCE) {
			if (imbalance*this_load <= 100*load) {
				schedstat_inc(this_sd, ttwu_move_balance);
				schedstat_inc(p, se.nr_wakeups_passive);
				goto out_set_cpu;
			}
		}
	}

	new_cpu = cpu; /* Could not wake to this_cpu. Wake to cpu instead */
out_set_cpu:
	return wake_idle(new_cpu, p);
}