static __inline__ int cbq_dump_rate(struct sk_buff *skb, struct cbq_class *cl)
{
	unsigned char	 *b = skb->tail;

	RTA_PUT(skb, TCA_CBQ_RATE, sizeof(cl->R_tab->rate), &cl->R_tab->rate);
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}