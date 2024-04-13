static unsigned long cbq_undelay_prio(struct cbq_sched_data *q, int prio)
{
	struct cbq_class *cl;
	struct cbq_class *cl_prev = q->active[prio];
	unsigned long now = jiffies;
	unsigned long sched = now;

	if (cl_prev == NULL)
		return now;

	do {
		cl = cl_prev->next_alive;
		if ((long)(now - cl->penalized) > 0) {
			cl_prev->next_alive = cl->next_alive;
			cl->next_alive = NULL;
			cl->cpriority = cl->priority;
			cl->delayed = 0;
			cbq_activate_class(cl);

			if (cl == q->active[prio]) {
				q->active[prio] = cl_prev;
				if (cl == q->active[prio]) {
					q->active[prio] = NULL;
					return 0;
				}
			}

			cl = cl_prev->next_alive;
		} else if ((long)(sched - cl->penalized) > 0)
			sched = cl->penalized;
	} while ((cl_prev = cl) != q->active[prio]);

	return (long)(sched - now);
}