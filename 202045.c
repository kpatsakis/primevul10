cbq_enqueue(struct sk_buff *skb, struct Qdisc *sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	int len = skb->len;
	int ret;
	struct cbq_class *cl = cbq_classify(skb, sch, &ret);

#ifdef CONFIG_NET_CLS_POLICE
	q->rx_class = cl;
#endif
	if (cl == NULL) {
		if (ret == NET_XMIT_DROP)
			sch->qstats.drops++;
		kfree_skb(skb);
		return ret;
	}

#ifdef CONFIG_NET_CLS_POLICE
	cl->q->__parent = sch;
#endif
	if ((ret = cl->q->enqueue(skb, cl->q)) == NET_XMIT_SUCCESS) {
		sch->q.qlen++;
		sch->bstats.packets++;
		sch->bstats.bytes+=len;
		cbq_mark_toplevel(q, cl);
		if (!cl->next_alive)
			cbq_activate_class(cl);
		return ret;
	}

	sch->qstats.drops++;
	cbq_mark_toplevel(q, cl);
	cl->qstats.drops++;
	return ret;
}