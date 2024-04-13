struct powercap_control_type *powercap_register_control_type(
				struct powercap_control_type *control_type,
				const char *name,
				const struct powercap_control_type_ops *ops)
{
	int result;

	if (!name)
		return ERR_PTR(-EINVAL);
	if (control_type) {
		if (!ops || !ops->release)
			return ERR_PTR(-EINVAL);
		memset(control_type, 0, sizeof(*control_type));
	} else {
		control_type = kzalloc(sizeof(*control_type), GFP_KERNEL);
		if (!control_type)
			return ERR_PTR(-ENOMEM);
		control_type->allocated = true;
	}
	mutex_init(&control_type->lock);
	control_type->ops = ops;
	INIT_LIST_HEAD(&control_type->node);
	control_type->dev.class = &powercap_class;
	dev_set_name(&control_type->dev, "%s", name);
	result = device_register(&control_type->dev);
	if (result) {
		if (control_type->allocated)
			kfree(control_type);
		return ERR_PTR(result);
	}
	idr_init(&control_type->idr);

	mutex_lock(&powercap_cntrl_list_lock);
	list_add_tail(&control_type->node, &powercap_cntrl_list);
	mutex_unlock(&powercap_cntrl_list_lock);

	return control_type;
}