tcf_exts_destroy(struct tcf_proto *tp, struct tcf_exts *exts)
{
#ifdef CONFIG_NET_CLS_ACT
	if (exts->action) {
		tcf_action_destroy(exts->action, TCA_ACT_UNBIND);
		exts->action = NULL;
	}
#elif defined CONFIG_NET_CLS_POLICE
	if (exts->police) {
		tcf_police_release(exts->police, TCA_ACT_UNBIND);
		exts->police = NULL;
	}
#endif
}