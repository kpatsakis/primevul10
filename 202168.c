cbq_under_limit(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);
	struct cbq_class *this_cl = cl;

	if (cl->tparent == NULL)
		return cl;

	if (PSCHED_IS_PASTPERFECT(cl->undertime) ||
	    !PSCHED_TLESS(q->now, cl->undertime)) {
		cl->delayed = 0;
		return cl;
	}

	do {
		/* It is very suspicious place. Now overlimit
		   action is generated for not bounded classes
		   only if link is completely congested.
		   Though it is in agree with ancestor-only paradigm,
		   it looks very stupid. Particularly,
		   it means that this chunk of code will either
		   never be called or result in strong amplification
		   of burstiness. Dangerous, silly, and, however,
		   no another solution exists.
		 */
		if ((cl = cl->borrow) == NULL) {
			this_cl->qstats.overlimits++;
			this_cl->overlimit(this_cl);
			return NULL;
		}
		if (cl->level > q->toplevel)
			return NULL;
	} while (!PSCHED_IS_PASTPERFECT(cl->undertime) &&
		 PSCHED_TLESS(q->now, cl->undertime));

	cl->delayed = 0;
	return cl;
}