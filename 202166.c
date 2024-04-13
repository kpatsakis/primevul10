static unsigned int cbq_drop(struct Qdisc* sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl, *cl_head;
	int prio;
	unsigned int len;

	for (prio = TC_CBQ_MAXPRIO; prio >= 0; prio--) {
		if ((cl_head = q->active[prio]) == NULL)
			continue;

		cl = cl_head;
		do {
			if (cl->q->ops->drop && (len = cl->q->ops->drop(cl->q))) {
				sch->q.qlen--;
				return len;
			}
		} while ((cl = cl->next_alive) != cl_head);
	}
	return 0;
}