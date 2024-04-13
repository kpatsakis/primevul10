tcf_exts_change(struct tcf_proto *tp, struct tcf_exts *dst,
	        struct tcf_exts *src)
{
#ifdef CONFIG_NET_CLS_ACT
	if (src->action) {
		struct tc_action *act;
		tcf_tree_lock(tp);
		act = xchg(&dst->action, src->action);
		tcf_tree_unlock(tp);
		if (act)
			tcf_action_destroy(act, TCA_ACT_UNBIND);
	}
#elif defined CONFIG_NET_CLS_POLICE
	if (src->police) {
		struct tcf_police *p;
		tcf_tree_lock(tp);
		p = xchg(&dst->police, src->police);
		tcf_tree_unlock(tp);
		if (p)
			tcf_police_release(p, TCA_ACT_UNBIND);
	}
#endif
}