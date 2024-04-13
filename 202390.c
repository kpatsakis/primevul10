struct tc_action *tcf_action_init_1(struct rtattr *rta, struct rtattr *est,
                                    char *name, int ovr, int bind, int *err)
{
	struct tc_action *a;
	struct tc_action_ops *a_o;
	char act_name[IFNAMSIZ];
	struct rtattr *tb[TCA_ACT_MAX+1];
	struct rtattr *kind;

	*err = -EINVAL;

	if (name == NULL) {
		if (rtattr_parse_nested(tb, TCA_ACT_MAX, rta) < 0)
			goto err_out;
		kind = tb[TCA_ACT_KIND-1];
		if (kind == NULL)
			goto err_out;
		if (rtattr_strlcpy(act_name, kind, IFNAMSIZ) >= IFNAMSIZ)
			goto err_out;
	} else {
		if (strlcpy(act_name, name, IFNAMSIZ) >= IFNAMSIZ)
			goto err_out;
	}

	a_o = tc_lookup_action_n(act_name);
	if (a_o == NULL) {
#ifdef CONFIG_KMOD
		rtnl_unlock();
		request_module(act_name);
		rtnl_lock();

		a_o = tc_lookup_action_n(act_name);

		/* We dropped the RTNL semaphore in order to
		 * perform the module load.  So, even if we
		 * succeeded in loading the module we have to
		 * tell the caller to replay the request.  We
		 * indicate this using -EAGAIN.
		 */
		if (a_o != NULL) {
			*err = -EAGAIN;
			goto err_mod;
		}
#endif
		goto err_out;
	}

	*err = -ENOMEM;
	a = kmalloc(sizeof(*a), GFP_KERNEL);
	if (a == NULL)
		goto err_mod;
	memset(a, 0, sizeof(*a));

	/* backward compatibility for policer */
	if (name == NULL)
		*err = a_o->init(tb[TCA_ACT_OPTIONS-1], est, a, ovr, bind);
	else
		*err = a_o->init(rta, est, a, ovr, bind);
	if (*err < 0)
		goto err_free;

	/* module count goes up only when brand new policy is created
	   if it exists and is only bound to in a_o->init() then
	   ACT_P_CREATED is not returned (a zero is).
	*/
	if (*err != ACT_P_CREATED)
		module_put(a_o->owner);
	a->ops = a_o;
	DPRINTK("tcf_action_init_1: successfull %s\n", act_name);

	*err = 0;
	return a;

err_free:
	kfree(a);
err_mod:
	module_put(a_o->owner);
err_out:
	return NULL;
}