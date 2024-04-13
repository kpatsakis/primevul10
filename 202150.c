static void cbq_ovl_lowprio(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);

	cl->penalized = jiffies + cl->penalty;

	if (cl->cpriority != cl->priority2) {
		cl->cpriority = cl->priority2;
		q->pmask |= (1<<cl->cpriority);
		cl->xstats.overactions++;
	}
	cbq_ovl_classic(cl);
}