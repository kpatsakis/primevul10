static ssize_t name_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct powercap_zone *power_zone = to_powercap_zone(dev);

	return sprintf(buf, "%s\n", power_zone->name);
}