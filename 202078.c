static __inline__ int cbq_dump_fopt(struct sk_buff *skb, struct cbq_class *cl)
{
	unsigned char	 *b = skb->tail;
	struct tc_cbq_fopt opt;

	if (cl->split || cl->defmap) {
		opt.split = cl->split ? cl->split->classid : 0;
		opt.defmap = cl->defmap;
		opt.defchange = ~0;
		RTA_PUT(skb, TCA_CBQ_FOPT, sizeof(opt), &opt);
	}
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}