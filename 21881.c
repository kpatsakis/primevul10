static void tcf_act_put_cookie(struct flow_action_entry *entry)
{
	flow_action_cookie_destroy(entry->cookie);
}