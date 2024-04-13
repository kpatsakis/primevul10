int tc_setup_action(struct flow_action *flow_action,
		    struct tc_action *actions[])
{
	int i, j, index, err = 0;
	struct tc_action *act;

	BUILD_BUG_ON(TCA_ACT_HW_STATS_ANY != FLOW_ACTION_HW_STATS_ANY);
	BUILD_BUG_ON(TCA_ACT_HW_STATS_IMMEDIATE != FLOW_ACTION_HW_STATS_IMMEDIATE);
	BUILD_BUG_ON(TCA_ACT_HW_STATS_DELAYED != FLOW_ACTION_HW_STATS_DELAYED);

	if (!actions)
		return 0;

	j = 0;
	tcf_act_for_each_action(i, act, actions) {
		struct flow_action_entry *entry;

		entry = &flow_action->entries[j];
		spin_lock_bh(&act->tcfa_lock);
		err = tcf_act_get_cookie(entry, act);
		if (err)
			goto err_out_locked;

		entry->hw_stats = tc_act_hw_stats(act->hw_stats);
		entry->hw_index = act->tcfa_index;
		index = 0;
		err = tc_setup_offload_act(act, entry, &index);
		if (!err)
			j += index;
		else
			goto err_out_locked;
		spin_unlock_bh(&act->tcfa_lock);
	}

err_out:
	if (err)
		tc_cleanup_offload_action(flow_action);

	return err;
err_out_locked:
	spin_unlock_bh(&act->tcfa_lock);
	goto err_out;
}