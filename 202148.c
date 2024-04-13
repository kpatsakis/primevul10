static int cbq_delete(struct Qdisc *sch, unsigned long arg)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl = (struct cbq_class*)arg;

	if (cl->filters || cl->children || cl == &q->link)
		return -EBUSY;

	sch_tree_lock(sch);

	if (cl->next_alive)
		cbq_deactivate_class(cl);

	if (q->tx_borrowed == cl)
		q->tx_borrowed = q->tx_class;
	if (q->tx_class == cl) {
		q->tx_class = NULL;
		q->tx_borrowed = NULL;
	}
#ifdef CONFIG_NET_CLS_POLICE
	if (q->rx_class == cl)
		q->rx_class = NULL;
#endif

	cbq_unlink_class(cl);
	cbq_adjust_levels(cl->tparent);
	cl->defmap = 0;
	cbq_sync_defmap(cl);

	cbq_rmprio(q, cl);
	sch_tree_unlock(sch);

	if (--cl->refcnt == 0)
		cbq_destroy_class(sch, cl);

	return 0;
}