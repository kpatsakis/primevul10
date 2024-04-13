int powercap_unregister_control_type(struct powercap_control_type *control_type)
{
	struct powercap_control_type *pos = NULL;

	if (control_type->nr_zones) {
		dev_err(&control_type->dev, "Zones of this type still not freed\n");
		return -EINVAL;
	}
	mutex_lock(&powercap_cntrl_list_lock);
	list_for_each_entry(pos, &powercap_cntrl_list, node) {
		if (pos == control_type) {
			list_del(&control_type->node);
			mutex_unlock(&powercap_cntrl_list_lock);
			device_unregister(&control_type->dev);
			return 0;
		}
	}
	mutex_unlock(&powercap_cntrl_list_lock);

	return -ENODEV;
}