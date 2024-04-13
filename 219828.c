static int seed_constraint_attributes(void)
{
	int i;
	int ret;

	for (i = 0; i < MAX_CONSTRAINTS_PER_ZONE; ++i) {
		ret = create_constraint_attribute(i, "power_limit_uw",
					S_IWUSR | S_IRUGO,
					&constraint_attrs[i].power_limit_attr,
					show_constraint_power_limit_uw,
					store_constraint_power_limit_uw);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "time_window_us",
					S_IWUSR | S_IRUGO,
					&constraint_attrs[i].time_window_attr,
					show_constraint_time_window_us,
					store_constraint_time_window_us);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "name", S_IRUGO,
				&constraint_attrs[i].name_attr,
				show_constraint_name,
				NULL);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "max_power_uw", S_IRUGO,
				&constraint_attrs[i].max_power_attr,
				show_constraint_max_power_uw,
				NULL);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "min_power_uw", S_IRUGO,
				&constraint_attrs[i].min_power_attr,
				show_constraint_min_power_uw,
				NULL);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "max_time_window_us",
				S_IRUGO,
				&constraint_attrs[i].max_time_window_attr,
				show_constraint_max_time_window_us,
				NULL);
		if (ret)
			goto err_alloc;
		ret = create_constraint_attribute(i, "min_time_window_us",
				S_IRUGO,
				&constraint_attrs[i].min_time_window_attr,
				show_constraint_min_time_window_us,
				NULL);
		if (ret)
			goto err_alloc;

	}

	return 0;

err_alloc:
	free_constraint_attributes();

	return ret;
}