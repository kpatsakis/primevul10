tcf_action_add(struct rtattr *rta, struct nlmsghdr *n, u32 pid, int ovr)
{
	int ret = 0;
	struct tc_action *act;
	struct tc_action *a;
	u32 seq = n->nlmsg_seq;

	act = tcf_action_init(rta, NULL, NULL, ovr, 0, &ret);
	if (act == NULL)
		goto done;

	/* dump then free all the actions after update; inserted policy
	 * stays intact
	 * */
	ret = tcf_add_notify(act, pid, seq, RTM_NEWACTION, n->nlmsg_flags);
	for (a = act; a; a = act) {
		act = a->next;
		kfree(a);
	}
done:
	return ret;
}