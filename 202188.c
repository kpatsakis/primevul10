cbq_dump_class(struct Qdisc *sch, unsigned long arg,
	       struct sk_buff *skb, struct tcmsg *tcm)
{
	struct cbq_class *cl = (struct cbq_class*)arg;
	unsigned char	 *b = skb->tail;
	struct rtattr *rta;

	if (cl->tparent)
		tcm->tcm_parent = cl->tparent->classid;
	else
		tcm->tcm_parent = TC_H_ROOT;
	tcm->tcm_handle = cl->classid;
	tcm->tcm_info = cl->q->handle;

	rta = (struct rtattr*)b;
	RTA_PUT(skb, TCA_OPTIONS, 0, NULL);
	if (cbq_dump_attr(skb, cl) < 0)
		goto rtattr_failure;
	rta->rta_len = skb->tail - b;
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}