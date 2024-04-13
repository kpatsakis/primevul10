cbq_destroy(struct Qdisc* sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl;
	unsigned h;

#ifdef CONFIG_NET_CLS_POLICE
	q->rx_class = NULL;
#endif
	/*
	 * Filters must be destroyed first because we don't destroy the
	 * classes from root to leafs which means that filters can still
	 * be bound to classes which have been destroyed already. --TGR '04
	 */
	for (h = 0; h < 16; h++)
		for (cl = q->classes[h]; cl; cl = cl->next)
			cbq_destroy_filters(cl);

	for (h = 0; h < 16; h++) {
		struct cbq_class *next;

		for (cl = q->classes[h]; cl; cl = next) {
			next = cl->next;
			cbq_destroy_class(sch, cl);
		}
	}
}