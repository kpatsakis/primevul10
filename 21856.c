struct sk_buff *tcf_qevent_handle(struct tcf_qevent *qe, struct Qdisc *sch, struct sk_buff *skb,
				  struct sk_buff **to_free, int *ret)
{
	struct tcf_result cl_res;
	struct tcf_proto *fl;

	if (!qe->info.block_index)
		return skb;

	fl = rcu_dereference_bh(qe->filter_chain);

	switch (tcf_classify(skb, NULL, fl, &cl_res, false)) {
	case TC_ACT_SHOT:
		qdisc_qstats_drop(sch);
		__qdisc_drop(skb, to_free);
		*ret = __NET_XMIT_BYPASS;
		return NULL;
	case TC_ACT_STOLEN:
	case TC_ACT_QUEUED:
	case TC_ACT_TRAP:
		__qdisc_drop(skb, to_free);
		*ret = __NET_XMIT_STOLEN;
		return NULL;
	case TC_ACT_REDIRECT:
		skb_do_redirect(skb);
		*ret = __NET_XMIT_STOLEN;
		return NULL;
	}

	return skb;
}