int tc_setup_offload_action(struct flow_action *flow_action,
			    const struct tcf_exts *exts)
{
#ifdef CONFIG_NET_CLS_ACT
	if (!exts)
		return 0;

	return tc_setup_action(flow_action, exts->actions);
#else
	return 0;
#endif
}