static int cbq_set_wrr(struct cbq_class *cl, struct tc_cbq_wrropt *wrr)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);

	if (wrr->allot)
		cl->allot = wrr->allot;
	if (wrr->weight)
		cl->weight = wrr->weight;
	if (wrr->priority) {
		cl->priority = wrr->priority-1;
		cl->cpriority = cl->priority;
		if (cl->priority >= cl->priority2)
			cl->priority2 = TC_CBQ_MAXPRIO-1;
	}

	cbq_addprio(q, cl);
	return 0;
}