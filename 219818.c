static ssize_t enabled_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	bool mode = true;

	/* Default is enabled */
	if (dev->parent) {
		struct powercap_zone *power_zone = to_powercap_zone(dev);
		if (power_zone->ops->get_enable)
			if (power_zone->ops->get_enable(power_zone, &mode))
				mode = false;
	} else {
		struct powercap_control_type *control_type =
						to_powercap_control_type(dev);
		if (control_type->ops && control_type->ops->get_enable)
			if (control_type->ops->get_enable(control_type, &mode))
				mode = false;
	}

	return sprintf(buf, "%d\n", mode);
}