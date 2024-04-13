cbq_reset(struct Qdisc* sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl;
	int prio;
	unsigned h;

	q->activemask = 0;
	q->pmask = 0;
	q->tx_class = NULL;
	q->tx_borrowed = NULL;
	del_timer(&q->wd_timer);
	del_timer(&q->delay_timer);
	q->toplevel = TC_CBQ_MAXLEVEL;
	PSCHED_GET_TIME(q->now);
	q->now_rt = q->now;

	for (prio = 0; prio <= TC_CBQ_MAXPRIO; prio++)
		q->active[prio] = NULL;

	for (h = 0; h < 16; h++) {
		for (cl = q->classes[h]; cl; cl = cl->next) {
			qdisc_reset(cl->q);

			cl->next_alive = NULL;
			PSCHED_SET_PASTPERFECT(cl->undertime);
			cl->avgidle = cl->maxidle;
			cl->deficit = cl->quantum;
			cl->cpriority = cl->priority;
		}
	}
	sch->q.qlen = 0;
}