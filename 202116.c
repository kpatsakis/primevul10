static void cbq_deactivate_class(struct cbq_class *this)
{
	struct cbq_sched_data *q = qdisc_priv(this->qdisc);
	int prio = this->cpriority;
	struct cbq_class *cl;
	struct cbq_class *cl_prev = q->active[prio];

	do {
		cl = cl_prev->next_alive;
		if (cl == this) {
			cl_prev->next_alive = cl->next_alive;
			cl->next_alive = NULL;

			if (cl == q->active[prio]) {
				q->active[prio] = cl_prev;
				if (cl == q->active[prio]) {
					q->active[prio] = NULL;
					q->activemask &= ~(1<<prio);
					return;
				}
			}

			cl = cl_prev->next_alive;
			return;
		}
	} while ((cl_prev = cl) != q->active[prio]);
}