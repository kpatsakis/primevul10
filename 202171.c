cbq_dequeue_1(struct Qdisc *sch)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct sk_buff *skb;
	unsigned activemask;

	activemask = q->activemask&0xFF;
	while (activemask) {
		int prio = ffz(~activemask);
		activemask &= ~(1<<prio);
		skb = cbq_dequeue_prio(sch, prio);
		if (skb)
			return skb;
	}
	return NULL;
}