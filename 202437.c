tca_action_gd(struct rtattr *rta, struct nlmsghdr *n, u32 pid, int event)
{
	int i, ret = 0;
	struct rtattr *tb[TCA_ACT_MAX_PRIO+1];
	struct tc_action *head = NULL, *act, *act_prev = NULL;

	if (rtattr_parse_nested(tb, TCA_ACT_MAX_PRIO, rta) < 0)
		return -EINVAL;

	if (event == RTM_DELACTION && n->nlmsg_flags&NLM_F_ROOT) {
		if (tb[0] != NULL && tb[1] == NULL)
			return tca_action_flush(tb[0], n, pid);
	}

	for (i=0; i < TCA_ACT_MAX_PRIO && tb[i]; i++) {
		act = tcf_action_get_1(tb[i], n, pid, &ret);
		if (act == NULL)
			goto err;
		act->order = i+1;

		if (head == NULL)
			head = act;
		else
			act_prev->next = act;
		act_prev = act;
	}

	if (event == RTM_GETACTION)
		ret = act_get_notify(pid, n, head, event);
	else { /* delete */
		struct sk_buff *skb;

		skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
		if (!skb) {
			ret = -ENOBUFS;
			goto err;
		}

		if (tca_get_fill(skb, head, pid, n->nlmsg_seq, 0, event,
		                 0, 1) <= 0) {
			kfree_skb(skb);
			ret = -EINVAL;
			goto err;
		}

		/* now do the delete */
		tcf_action_destroy(head, 0);
		ret = rtnetlink_send(skb, pid, RTMGRP_TC,
		                     n->nlmsg_flags&NLM_F_ECHO);
		if (ret > 0)
			return 0;
		return ret;
	}
err:
	cleanup_a(head);
	return ret;
}