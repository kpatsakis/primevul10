int tcf_exts_dump(struct sk_buff *skb, struct tcf_exts *exts,
		  const struct tcf_ext_map *map)
{
#ifdef CONFIG_NET_CLS_ACT
	if (map->action && exts->action) {
		/*
		 * again for backward compatible mode - we want
		 * to work with both old and new modes of entering
		 * tc data even if iproute2  was newer - jhs
		 */
		struct nlattr *nest;

		if (exts->action->type != TCA_OLD_COMPAT) {
			nest = nla_nest_start(skb, map->action);
			if (nest == NULL)
				goto nla_put_failure;
			if (tcf_action_dump(skb, exts->action, 0, 0) < 0)
				goto nla_put_failure;
			nla_nest_end(skb, nest);
		} else if (map->police) {
			nest = nla_nest_start(skb, map->police);
			if (nest == NULL)
				goto nla_put_failure;
			if (tcf_action_dump_old(skb, exts->action, 0, 0) < 0)
				goto nla_put_failure;
			nla_nest_end(skb, nest);
		}
	}
#endif
	return 0;
nla_put_failure: __attribute__ ((unused))
	return -1;
}