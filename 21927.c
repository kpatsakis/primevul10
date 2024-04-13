void tcf_exts_destroy(struct tcf_exts *exts)
{
#ifdef CONFIG_NET_CLS_ACT
	if (exts->actions) {
		tcf_action_destroy(exts->actions, TCA_ACT_UNBIND);
		kfree(exts->actions);
	}
	exts->nr_actions = 0;
#endif
}