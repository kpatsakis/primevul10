int tcf_exts_validate(struct tcf_proto *tp, struct nlattr **tb,
		  struct nlattr *rate_tlv, struct tcf_exts *exts,
		  const struct tcf_ext_map *map)
{
	memset(exts, 0, sizeof(*exts));

#ifdef CONFIG_NET_CLS_ACT
	{
		struct tc_action *act;

		if (map->police && tb[map->police]) {
			act = tcf_action_init_1(tb[map->police], rate_tlv,
						"police", TCA_ACT_NOREPLACE,
						TCA_ACT_BIND);
			if (IS_ERR(act))
				return PTR_ERR(act);

			act->type = TCA_OLD_COMPAT;
			exts->action = act;
		} else if (map->action && tb[map->action]) {
			act = tcf_action_init(tb[map->action], rate_tlv, NULL,
					      TCA_ACT_NOREPLACE, TCA_ACT_BIND);
			if (IS_ERR(act))
				return PTR_ERR(act);

			exts->action = act;
		}
	}
#else
	if ((map->action && tb[map->action]) ||
	    (map->police && tb[map->police]))
		return -EOPNOTSUPP;
#endif

	return 0;
}