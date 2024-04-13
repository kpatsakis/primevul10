static void tfilter_notify_chain(struct net *net, struct sk_buff *oskb,
				 struct tcf_block *block, struct Qdisc *q,
				 u32 parent, struct nlmsghdr *n,
				 struct tcf_chain *chain, int event)
{
	struct tcf_proto *tp;

	for (tp = tcf_get_next_proto(chain, NULL);
	     tp; tp = tcf_get_next_proto(chain, tp))
		tfilter_notify(net, oskb, n, tp, block,
			       q, parent, NULL, event, false, true);
}