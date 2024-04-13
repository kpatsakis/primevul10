int tcf_exts_validate_ex(struct net *net, struct tcf_proto *tp, struct nlattr **tb,
			 struct nlattr *rate_tlv, struct tcf_exts *exts,
			 u32 flags, u32 fl_flags, struct netlink_ext_ack *extack)
{
#ifdef CONFIG_NET_CLS_ACT
	{
		int init_res[TCA_ACT_MAX_PRIO] = {};
		struct tc_action *act;
		size_t attr_size = 0;

		if (exts->police && tb[exts->police]) {
			struct tc_action_ops *a_o;

			a_o = tc_action_load_ops(tb[exts->police], true,
						 !(flags & TCA_ACT_FLAGS_NO_RTNL),
						 extack);
			if (IS_ERR(a_o))
				return PTR_ERR(a_o);
			flags |= TCA_ACT_FLAGS_POLICE | TCA_ACT_FLAGS_BIND;
			act = tcf_action_init_1(net, tp, tb[exts->police],
						rate_tlv, a_o, init_res, flags,
						extack);
			module_put(a_o->owner);
			if (IS_ERR(act))
				return PTR_ERR(act);

			act->type = exts->type = TCA_OLD_COMPAT;
			exts->actions[0] = act;
			exts->nr_actions = 1;
			tcf_idr_insert_many(exts->actions);
		} else if (exts->action && tb[exts->action]) {
			int err;

			flags |= TCA_ACT_FLAGS_BIND;
			err = tcf_action_init(net, tp, tb[exts->action],
					      rate_tlv, exts->actions, init_res,
					      &attr_size, flags, fl_flags,
					      extack);
			if (err < 0)
				return err;
			exts->nr_actions = err;
		}
	}
#else
	if ((exts->action && tb[exts->action]) ||
	    (exts->police && tb[exts->police])) {
		NL_SET_ERR_MSG(extack, "Classifier actions are not supported per compile options (CONFIG_NET_CLS_ACT)");
		return -EOPNOTSUPP;
	}
#endif

	return 0;
}