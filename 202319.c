tcf_action_get_1(struct rtattr *rta, struct nlmsghdr *n, u32 pid, int *err)
{
	struct rtattr *tb[TCA_ACT_MAX+1];
	struct tc_action *a;
	int index;

	*err = -EINVAL;
	if (rtattr_parse_nested(tb, TCA_ACT_MAX, rta) < 0)
		return NULL;

	if (tb[TCA_ACT_INDEX - 1] == NULL ||
	    RTA_PAYLOAD(tb[TCA_ACT_INDEX - 1]) < sizeof(index))
		return NULL;
	index = *(int *)RTA_DATA(tb[TCA_ACT_INDEX - 1]);

	*err = -ENOMEM;
	a = kmalloc(sizeof(struct tc_action), GFP_KERNEL);
	if (a == NULL)
		return NULL;
	memset(a, 0, sizeof(struct tc_action));

	*err = -EINVAL;
	a->ops = tc_lookup_action(tb[TCA_ACT_KIND - 1]);
	if (a->ops == NULL)
		goto err_free;
	if (a->ops->lookup == NULL)
		goto err_mod;
	*err = -ENOENT;
	if (a->ops->lookup(a, index) == 0)
		goto err_mod;

	module_put(a->ops->owner);
	*err = 0;
	return a;
err_mod:
	module_put(a->ops->owner);
err_free:
	kfree(a);
	return NULL;
}