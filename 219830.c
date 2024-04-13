static void free_constraint_attributes(void)
{
	int i;

	for (i = 0; i < MAX_CONSTRAINTS_PER_ZONE; ++i) {
		kfree(constraint_attrs[i].power_limit_attr.attr.name);
		kfree(constraint_attrs[i].time_window_attr.attr.name);
		kfree(constraint_attrs[i].name_attr.attr.name);
		kfree(constraint_attrs[i].max_power_attr.attr.name);
		kfree(constraint_attrs[i].min_power_attr.attr.name);
		kfree(constraint_attrs[i].max_time_window_attr.attr.name);
		kfree(constraint_attrs[i].min_time_window_attr.attr.name);
	}
}