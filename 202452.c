int tcf_exts_dump_stats(struct sk_buff *skb, struct tcf_exts *exts,
			const struct tcf_ext_map *map)
{
#ifdef CONFIG_NET_CLS_ACT
	if (exts->action)
		if (tcf_action_copy_stats(skb, exts->action, 1) < 0)
			goto nla_put_failure;
#endif
	return 0;
nla_put_failure: __attribute__ ((unused))
	return -1;
}