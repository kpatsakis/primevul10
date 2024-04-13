static ssize_t enabled_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf,  size_t len)
{
	bool mode;

	if (strtobool(buf, &mode))
		return -EINVAL;
	if (dev->parent) {
		struct powercap_zone *power_zone = to_powercap_zone(dev);
		if (power_zone->ops->set_enable)
			if (!power_zone->ops->set_enable(power_zone, mode))
				return len;
	} else {
		struct powercap_control_type *control_type =
						to_powercap_control_type(dev);
		if (control_type->ops && control_type->ops->set_enable)
			if (!control_type->ops->set_enable(control_type, mode))
				return len;
	}

	return -ENOSYS;
}