int tcf_classify(struct sk_buff *skb,
		 const struct tcf_block *block,
		 const struct tcf_proto *tp,
		 struct tcf_result *res, bool compat_mode)
{
#if !IS_ENABLED(CONFIG_NET_TC_SKB_EXT)
	u32 last_executed_chain = 0;

	return __tcf_classify(skb, tp, tp, res, compat_mode,
			      &last_executed_chain);
#else
	u32 last_executed_chain = tp ? tp->chain->index : 0;
	const struct tcf_proto *orig_tp = tp;
	struct tc_skb_ext *ext;
	int ret;

	if (block) {
		ext = skb_ext_find(skb, TC_SKB_EXT);

		if (ext && ext->chain) {
			struct tcf_chain *fchain;

			fchain = tcf_chain_lookup_rcu(block, ext->chain);
			if (!fchain)
				return TC_ACT_SHOT;

			/* Consume, so cloned/redirect skbs won't inherit ext */
			skb_ext_del(skb, TC_SKB_EXT);

			tp = rcu_dereference_bh(fchain->filter_chain);
			last_executed_chain = fchain->index;
		}
	}

	ret = __tcf_classify(skb, tp, orig_tp, res, compat_mode,
			     &last_executed_chain);

	/* If we missed on some chain */
	if (ret == TC_ACT_UNSPEC && last_executed_chain) {
		struct tc_skb_cb *cb = tc_skb_cb(skb);

		ext = tc_skb_ext_alloc(skb);
		if (WARN_ON_ONCE(!ext))
			return TC_ACT_SHOT;
		ext->chain = last_executed_chain;
		ext->mru = cb->mru;
		ext->post_ct = cb->post_ct;
		ext->post_ct_snat = cb->post_ct_snat;
		ext->post_ct_dnat = cb->post_ct_dnat;
		ext->zone = cb->zone;
	}

	return ret;
#endif
}