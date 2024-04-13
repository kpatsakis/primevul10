static __inline__ int cbq_dump_wrr(struct sk_buff *skb, struct cbq_class *cl)
{
	unsigned char	 *b = skb->tail;
	struct tc_cbq_wrropt opt;

	opt.flags = 0;
	opt.allot = cl->allot;
	opt.priority = cl->priority+1;
	opt.cpriority = cl->cpriority+1;
	opt.weight = cl->weight;
	RTA_PUT(skb, TCA_CBQ_WRROPT, sizeof(opt), &opt);
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}