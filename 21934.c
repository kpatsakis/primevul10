static inline int __tcf_classify(struct sk_buff *skb,
				 const struct tcf_proto *tp,
				 const struct tcf_proto *orig_tp,
				 struct tcf_result *res,
				 bool compat_mode,
				 u32 *last_executed_chain)
{
#ifdef CONFIG_NET_CLS_ACT
	const int max_reclassify_loop = 16;
	const struct tcf_proto *first_tp;
	int limit = 0;

reclassify:
#endif
	for (; tp; tp = rcu_dereference_bh(tp->next)) {
		__be16 protocol = skb_protocol(skb, false);
		int err;

		if (tp->protocol != protocol &&
		    tp->protocol != htons(ETH_P_ALL))
			continue;

		err = tp->classify(skb, tp, res);
#ifdef CONFIG_NET_CLS_ACT
		if (unlikely(err == TC_ACT_RECLASSIFY && !compat_mode)) {
			first_tp = orig_tp;
			*last_executed_chain = first_tp->chain->index;
			goto reset;
		} else if (unlikely(TC_ACT_EXT_CMP(err, TC_ACT_GOTO_CHAIN))) {
			first_tp = res->goto_tp;
			*last_executed_chain = err & TC_ACT_EXT_VAL_MASK;
			goto reset;
		}
#endif
		if (err >= 0)
			return err;
	}

	return TC_ACT_UNSPEC; /* signal: continue lookup */
#ifdef CONFIG_NET_CLS_ACT
reset:
	if (unlikely(limit++ >= max_reclassify_loop)) {
		net_notice_ratelimited("%u: reclassify loop, rule prio %u, protocol %02x\n",
				       tp->chain->block->index,
				       tp->prio & 0xffff,
				       ntohs(tp->protocol));
		return TC_ACT_SHOT;
	}

	tp = first_tp;
	goto reclassify;
#endif
}