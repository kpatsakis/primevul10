static int cbq_reshape_fail(struct sk_buff *skb, struct Qdisc *child)
{
	int len = skb->len;
	struct Qdisc *sch = child->__parent;
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl = q->rx_class;

	q->rx_class = NULL;

	if (cl && (cl = cbq_reclassify(skb, cl)) != NULL) {

		cbq_mark_toplevel(q, cl);

		q->rx_class = cl;
		cl->q->__parent = sch;

		if (cl->q->enqueue(skb, cl->q) == 0) {
			sch->q.qlen++;
			sch->bstats.packets++;
			sch->bstats.bytes+=len;
			if (!cl->next_alive)
				cbq_activate_class(cl);
			return 0;
		}
		sch->qstats.drops++;
		return 0;
	}

	sch->qstats.drops++;
	return -1;
}