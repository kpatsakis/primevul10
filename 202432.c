tcf_exts_dump(struct sk_buff *skb, struct tcf_exts *exts,
	      struct tcf_ext_map *map)
{
#ifdef CONFIG_NET_CLS_ACT
	if (map->action && exts->action) {
		/*
		 * again for backward compatible mode - we want
		 * to work with both old and new modes of entering
		 * tc data even if iproute2  was newer - jhs
		 */
		struct rtattr * p_rta = (struct rtattr*) skb->tail;

		if (exts->action->type != TCA_OLD_COMPAT) {
			RTA_PUT(skb, map->action, 0, NULL);
			if (tcf_action_dump(skb, exts->action, 0, 0) < 0)
				goto rtattr_failure;
			p_rta->rta_len = skb->tail - (u8*)p_rta;
		} else if (map->police) {
			RTA_PUT(skb, map->police, 0, NULL);
			if (tcf_action_dump_old(skb, exts->action, 0, 0) < 0)
				goto rtattr_failure;
			p_rta->rta_len = skb->tail - (u8*)p_rta;
		}
	}
#elif defined CONFIG_NET_CLS_POLICE
	if (map->police && exts->police) {
		struct rtattr * p_rta = (struct rtattr*) skb->tail;

		RTA_PUT(skb, map->police, 0, NULL);

		if (tcf_police_dump(skb, exts->police) < 0)
			goto rtattr_failure;

		p_rta->rta_len = skb->tail - (u8*)p_rta;
	}
#endif
	return 0;
rtattr_failure: __attribute__ ((unused))
	return -1;
}