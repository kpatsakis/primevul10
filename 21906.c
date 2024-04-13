void tc_cleanup_offload_action(struct flow_action *flow_action)
{
	struct flow_action_entry *entry;
	int i;

	flow_action_for_each(i, entry, flow_action) {
		tcf_act_put_cookie(entry);
		if (entry->destructor)
			entry->destructor(entry->destructor_priv);
	}
}