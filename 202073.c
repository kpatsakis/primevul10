cbq_requeue(struct sk_buff *skb, struct Qdisc *sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl;
	int ret;

	if ((cl = q->tx_class) == NULL) {
		kfree_skb(skb);
		sch->qstats.drops++;
		return NET_XMIT_CN;
	}
	q->tx_class = NULL;

	cbq_mark_toplevel(q, cl);

#ifdef CONFIG_NET_CLS_POLICE
	q->rx_class = cl;
	cl->q->__parent = sch;
#endif
	if ((ret = cl->q->ops->requeue(skb, cl->q)) == 0) {
		sch->q.qlen++;
		sch->qstats.requeues++;
		if (!cl->next_alive)
			cbq_activate_class(cl);
		return 0;
	}
	sch->qstats.drops++;
	cl->qstats.drops++;
	return ret;
}