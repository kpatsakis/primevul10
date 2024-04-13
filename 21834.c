static int tc_setup_offload_act(struct tc_action *act,
				struct flow_action_entry *entry,
				u32 *index_inc)
{
#ifdef CONFIG_NET_CLS_ACT
	if (act->ops->offload_act_setup)
		return act->ops->offload_act_setup(act, entry, index_inc, true);
	else
		return -EOPNOTSUPP;
#else
	return 0;
#endif
}