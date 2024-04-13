static __inline__ int cbq_dump_lss(struct sk_buff *skb, struct cbq_class *cl)
{
	unsigned char	 *b = skb->tail;
	struct tc_cbq_lssopt opt;

	opt.flags = 0;
	if (cl->borrow == NULL)
		opt.flags |= TCF_CBQ_LSS_BOUNDED;
	if (cl->share == NULL)
		opt.flags |= TCF_CBQ_LSS_ISOLATED;
	opt.ewma_log = cl->ewma_log;
	opt.level = cl->level;
	opt.avpkt = cl->avpkt;
	opt.maxidle = cl->maxidle;
	opt.minidle = (u32)(-cl->minidle);
	opt.offtime = cl->offtime;
	opt.change = ~0;
	RTA_PUT(skb, TCA_CBQ_LSSOPT, sizeof(opt), &opt);
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}