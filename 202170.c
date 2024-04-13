static void cbq_ovl_classic(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);
	psched_tdiff_t delay = PSCHED_TDIFF(cl->undertime, q->now);

	if (!cl->delayed) {
		delay += cl->offtime;

		/* 
		   Class goes to sleep, so that it will have no
		   chance to work avgidle. Let's forgive it 8)

		   BTW cbq-2.0 has a crap in this
		   place, apparently they forgot to shift it by cl->ewma_log.
		 */
		if (cl->avgidle < 0)
			delay -= (-cl->avgidle) - ((-cl->avgidle) >> cl->ewma_log);
		if (cl->avgidle < cl->minidle)
			cl->avgidle = cl->minidle;
		if (delay <= 0)
			delay = 1;
		PSCHED_TADD2(q->now, delay, cl->undertime);

		cl->xstats.overactions++;
		cl->delayed = 1;
	}
	if (q->wd_expires == 0 || q->wd_expires > delay)
		q->wd_expires = delay;

	/* Dirty work! We must schedule wakeups based on
	   real available rate, rather than leaf rate,
	   which may be tiny (even zero).
	 */
	if (q->toplevel == TC_CBQ_MAXLEVEL) {
		struct cbq_class *b;
		psched_tdiff_t base_delay = q->wd_expires;

		for (b = cl->borrow; b; b = b->borrow) {
			delay = PSCHED_TDIFF(b->undertime, q->now);
			if (delay < base_delay) {
				if (delay <= 0)
					delay = 1;
				base_delay = delay;
			}
		}

		q->wd_expires = base_delay;
	}
}