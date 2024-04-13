static int create_constraints(struct powercap_zone *power_zone,
			int nr_constraints,
			const struct powercap_zone_constraint_ops *const_ops)
{
	int i;
	int ret = 0;
	int count;
	struct powercap_zone_constraint *pconst;

	if (!power_zone || !const_ops || !const_ops->get_power_limit_uw ||
					!const_ops->set_power_limit_uw ||
					!const_ops->get_time_window_us ||
					!const_ops->set_time_window_us)
		return -EINVAL;

	count = power_zone->zone_attr_count;
	for (i = 0; i < nr_constraints; ++i) {
		pconst = &power_zone->constraints[i];
		pconst->ops = const_ops;
		pconst->id = power_zone->const_id_cnt;
		power_zone->const_id_cnt++;
		power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].power_limit_attr.attr;
		power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].time_window_attr.attr;
		if (pconst->ops->get_name)
			power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].name_attr.attr;
		if (pconst->ops->get_max_power_uw)
			power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].max_power_attr.attr;
		if (pconst->ops->get_min_power_uw)
			power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].min_power_attr.attr;
		if (pconst->ops->get_max_time_window_us)
			power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].max_time_window_attr.attr;
		if (pconst->ops->get_min_time_window_us)
			power_zone->zone_dev_attrs[count++] =
				&constraint_attrs[i].min_time_window_attr.attr;
	}
	power_zone->zone_attr_count = count;

	return ret;
}