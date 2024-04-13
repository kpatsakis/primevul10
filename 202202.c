static void cbq_ovl_delay(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);
	psched_tdiff_t delay = PSCHED_TDIFF(cl->undertime, q->now);

	if (!cl->delayed) {
		unsigned long sched = jiffies;

		delay += cl->offtime;
		if (cl->avgidle < 0)
			delay -= (-cl->avgidle) - ((-cl->avgidle) >> cl->ewma_log);
		if (cl->avgidle < cl->minidle)
			cl->avgidle = cl->minidle;
		PSCHED_TADD2(q->now, delay, cl->undertime);

		if (delay > 0) {
			sched += PSCHED_US2JIFFIE(delay) + cl->penalty;
			cl->penalized = sched;
			cl->cpriority = TC_CBQ_MAXPRIO;
			q->pmask |= (1<<TC_CBQ_MAXPRIO);
			if (del_timer(&q->delay_timer) &&
			    (long)(q->delay_timer.expires - sched) > 0)
				q->delay_timer.expires = sched;
			add_timer(&q->delay_timer);
			cl->delayed = 1;
			cl->xstats.overactions++;
			return;
		}
		delay = 1;
	}
	if (q->wd_expires == 0 || q->wd_expires > delay)
		q->wd_expires = delay;
}