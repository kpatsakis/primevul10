static void cbq_walk(struct Qdisc *sch, struct qdisc_walker *arg)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	unsigned h;

	if (arg->stop)
		return;

	for (h = 0; h < 16; h++) {
		struct cbq_class *cl;

		for (cl = q->classes[h]; cl; cl = cl->next) {
			if (arg->count < arg->skip) {
				arg->count++;
				continue;
			}
			if (arg->fn(sch, (unsigned long)cl, arg) < 0) {
				arg->stop = 1;
				return;
			}
			arg->count++;
		}
	}
}