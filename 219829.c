struct powercap_zone *powercap_register_zone(
			struct powercap_zone *power_zone,
			struct powercap_control_type *control_type,
			const char *name,
			struct powercap_zone *parent,
			const struct powercap_zone_ops *ops,
			int nr_constraints,
			const struct powercap_zone_constraint_ops *const_ops)
{
	int result;
	int nr_attrs;

	if (!name || !control_type || !ops ||
			nr_constraints > MAX_CONSTRAINTS_PER_ZONE ||
			(!ops->get_energy_uj && !ops->get_power_uw) ||
			!control_type_valid(control_type))
		return ERR_PTR(-EINVAL);

	if (power_zone) {
		if (!ops->release)
			return ERR_PTR(-EINVAL);
		memset(power_zone, 0, sizeof(*power_zone));
	} else {
		power_zone = kzalloc(sizeof(*power_zone), GFP_KERNEL);
		if (!power_zone)
			return ERR_PTR(-ENOMEM);
		power_zone->allocated = true;
	}
	power_zone->ops = ops;
	power_zone->control_type_inst = control_type;
	if (!parent) {
		power_zone->dev.parent = &control_type->dev;
		power_zone->parent_idr = &control_type->idr;
	} else {
		power_zone->dev.parent = &parent->dev;
		power_zone->parent_idr = &parent->idr;
	}
	power_zone->dev.class = &powercap_class;

	mutex_lock(&control_type->lock);
	/* Using idr to get the unique id */
	result = idr_alloc(power_zone->parent_idr, NULL, 0, 0, GFP_KERNEL);
	if (result < 0)
		goto err_idr_alloc;

	power_zone->id = result;
	idr_init(&power_zone->idr);
	result = -ENOMEM;
	power_zone->name = kstrdup(name, GFP_KERNEL);
	if (!power_zone->name)
		goto err_name_alloc;
	dev_set_name(&power_zone->dev, "%s:%x",
					dev_name(power_zone->dev.parent),
					power_zone->id);
	power_zone->constraints = kcalloc(nr_constraints,
					  sizeof(*power_zone->constraints),
					  GFP_KERNEL);
	if (!power_zone->constraints)
		goto err_const_alloc;

	nr_attrs = nr_constraints * POWERCAP_CONSTRAINTS_ATTRS +
						POWERCAP_ZONE_MAX_ATTRS + 1;
	power_zone->zone_dev_attrs = kcalloc(nr_attrs, sizeof(void *),
					     GFP_KERNEL);
	if (!power_zone->zone_dev_attrs)
		goto err_attr_alloc;
	create_power_zone_common_attributes(power_zone);
	result = create_constraints(power_zone, nr_constraints, const_ops);
	if (result)
		goto err_dev_ret;

	power_zone->zone_dev_attrs[power_zone->zone_attr_count] = NULL;
	power_zone->dev_zone_attr_group.attrs = power_zone->zone_dev_attrs;
	power_zone->dev_attr_groups[0] = &power_zone->dev_zone_attr_group;
	power_zone->dev_attr_groups[1] = NULL;
	power_zone->dev.groups = power_zone->dev_attr_groups;
	result = device_register(&power_zone->dev);
	if (result)
		goto err_dev_ret;

	control_type->nr_zones++;
	mutex_unlock(&control_type->lock);

	return power_zone;

err_dev_ret:
	kfree(power_zone->zone_dev_attrs);
err_attr_alloc:
	kfree(power_zone->constraints);
err_const_alloc:
	kfree(power_zone->name);
err_name_alloc:
	idr_remove(power_zone->parent_idr, power_zone->id);
err_idr_alloc:
	if (power_zone->allocated)
		kfree(power_zone);
	mutex_unlock(&control_type->lock);

	return ERR_PTR(result);
}