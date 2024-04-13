int tcf_exts_terse_dump(struct sk_buff *skb, struct tcf_exts *exts)
{
#ifdef CONFIG_NET_CLS_ACT
	struct nlattr *nest;

	if (!exts->action || !tcf_exts_has_actions(exts))
		return 0;

	nest = nla_nest_start_noflag(skb, exts->action);
	if (!nest)
		goto nla_put_failure;

	if (tcf_action_dump(skb, exts->actions, 0, 0, true) < 0)
		goto nla_put_failure;
	nla_nest_end(skb, nest);
	return 0;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
#else
	return 0;
#endif
}