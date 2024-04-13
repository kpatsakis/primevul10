static int cbq_dump_attr(struct sk_buff *skb, struct cbq_class *cl)
{
	if (cbq_dump_lss(skb, cl) < 0 ||
	    cbq_dump_rate(skb, cl) < 0 ||
	    cbq_dump_wrr(skb, cl) < 0 ||
	    cbq_dump_ovl(skb, cl) < 0 ||
#ifdef CONFIG_NET_CLS_POLICE
	    cbq_dump_police(skb, cl) < 0 ||
#endif
	    cbq_dump_fopt(skb, cl) < 0)
		return -1;
	return 0;
}