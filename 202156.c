cbq_update_toplevel(struct cbq_sched_data *q, struct cbq_class *cl,
		    struct cbq_class *borrowed)
{
	if (cl && q->toplevel >= borrowed->level) {
		if (cl->q->q.qlen > 1) {
			do {
				if (PSCHED_IS_PASTPERFECT(borrowed->undertime)) {
					q->toplevel = borrowed->level;
					return;
				}
			} while ((borrowed=borrowed->borrow) != NULL);
		}
#if 0	
	/* It is not necessary now. Uncommenting it
	   will save CPU cycles, but decrease fairness.
	 */
		q->toplevel = TC_CBQ_MAXLEVEL;
#endif
	}
}