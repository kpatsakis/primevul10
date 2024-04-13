static int tcf_act_get_cookie(struct flow_action_entry *entry,
			      const struct tc_action *act)
{
	struct tc_cookie *cookie;
	int err = 0;

	rcu_read_lock();
	cookie = rcu_dereference(act->act_cookie);
	if (cookie) {
		entry->cookie = flow_action_cookie_create(cookie->data,
							  cookie->len,
							  GFP_ATOMIC);
		if (!entry->cookie)
			err = -ENOMEM;
	}
	rcu_read_unlock();
	return err;
}