int powercap_unregister_zone(struct powercap_control_type *control_type,
				struct powercap_zone *power_zone)
{
	if (!power_zone || !control_type)
		return -EINVAL;

	mutex_lock(&control_type->lock);
	control_type->nr_zones--;
	mutex_unlock(&control_type->lock);

	device_unregister(&power_zone->dev);

	return 0;
}