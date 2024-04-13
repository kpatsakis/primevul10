struct tc_action *tcf_action_init(struct rtattr *rta, struct rtattr *est,
                                  char *name, int ovr, int bind, int *err)
{
	struct rtattr *tb[TCA_ACT_MAX_PRIO+1];
	struct tc_action *head = NULL, *act, *act_prev = NULL;
	int i;

	if (rtattr_parse_nested(tb, TCA_ACT_MAX_PRIO, rta) < 0) {
		*err = -EINVAL;
		return head;
	}

	for (i=0; i < TCA_ACT_MAX_PRIO && tb[i]; i++) {
		act = tcf_action_init_1(tb[i], est, name, ovr, bind, err);
		if (act == NULL)
			goto err;
		act->order = i+1;

		if (head == NULL)
			head = act;
		else
			act_prev->next = act;
		act_prev = act;
	}
	return head;

err:
	if (head != NULL)
		tcf_action_destroy(head, bind);
	return NULL;
}