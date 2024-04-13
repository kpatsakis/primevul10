tcf_exts_validate(struct tcf_proto *tp, struct rtattr **tb,
	          struct rtattr *rate_tlv, struct tcf_exts *exts,
	          struct tcf_ext_map *map)
{
	memset(exts, 0, sizeof(*exts));
	
#ifdef CONFIG_NET_CLS_ACT
	{
		int err;
		struct tc_action *act;

		if (map->police && tb[map->police-1]) {
			act = tcf_action_init_1(tb[map->police-1], rate_tlv, "police",
				TCA_ACT_NOREPLACE, TCA_ACT_BIND, &err);
			if (act == NULL)
				return err;

			act->type = TCA_OLD_COMPAT;
			exts->action = act;
		} else if (map->action && tb[map->action-1]) {
			act = tcf_action_init(tb[map->action-1], rate_tlv, NULL,
				TCA_ACT_NOREPLACE, TCA_ACT_BIND, &err);
			if (act == NULL)
				return err;

			exts->action = act;
		}
	}
#elif defined CONFIG_NET_CLS_POLICE
	if (map->police && tb[map->police-1]) {
		struct tcf_police *p;
		
		p = tcf_police_locate(tb[map->police-1], rate_tlv);
		if (p == NULL)
			return -EINVAL;

		exts->police = p;
	} else if (map->action && tb[map->action-1])
		return -EOPNOTSUPP;
#else
	if ((map->action && tb[map->action-1]) ||
	    (map->police && tb[map->police-1]))
		return -EOPNOTSUPP;
#endif

	return 0;
}