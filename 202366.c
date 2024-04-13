tcf_exts_dump_stats(struct sk_buff *skb, struct tcf_exts *exts,
	            struct tcf_ext_map *map)
{
#ifdef CONFIG_NET_CLS_ACT
	if (exts->action)
		if (tcf_action_copy_stats(skb, exts->action, 1) < 0)
			goto rtattr_failure;
#elif defined CONFIG_NET_CLS_POLICE
	if (exts->police)
		if (tcf_police_dump_stats(skb, exts->police) < 0)
			goto rtattr_failure;
#endif
	return 0;
rtattr_failure: __attribute__ ((unused))
	return -1;
}