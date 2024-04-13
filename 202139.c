cbq_mark_toplevel(struct cbq_sched_data *q, struct cbq_class *cl)
{
	int toplevel = q->toplevel;

	if (toplevel > cl->level && !(cl->q->flags&TCQ_F_THROTTLED)) {
		psched_time_t now;
		psched_tdiff_t incr;

		PSCHED_GET_TIME(now);
		incr = PSCHED_TDIFF(now, q->now_rt);
		PSCHED_TADD2(q->now, incr, now);

		do {
			if (PSCHED_TLESS(cl->undertime, now)) {
				q->toplevel = cl->level;
				return;
			}
		} while ((cl=cl->borrow) != NULL && toplevel > cl->level);
	}
}