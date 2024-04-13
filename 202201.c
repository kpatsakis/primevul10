static int cbq_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	unsigned char	 *b = skb->tail;
	struct rtattr *rta;

	rta = (struct rtattr*)b;
	RTA_PUT(skb, TCA_OPTIONS, 0, NULL);
	if (cbq_dump_attr(skb, &q->link) < 0)
		goto rtattr_failure;
	rta->rta_len = skb->tail - b;
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}