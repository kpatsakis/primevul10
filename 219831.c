static ssize_t show_constraint_name(struct device *dev,
				struct device_attribute *dev_attr,
				char *buf)
{
	const char *name;
	struct powercap_zone *power_zone = to_powercap_zone(dev);
	int id;
	ssize_t len = -ENODATA;
	struct powercap_zone_constraint *pconst;

	if (!sscanf(dev_attr->attr.name, "constraint_%d_", &id))
		return -EINVAL;
	if (id >= power_zone->const_id_cnt)
		return -EINVAL;
	pconst = &power_zone->constraints[id];

	if (pconst && pconst->ops && pconst->ops->get_name) {
		name = pconst->ops->get_name(power_zone, id);
		if (name) {
			snprintf(buf, POWERCAP_CONSTRAINT_NAME_LEN,
								"%s\n", name);
			buf[POWERCAP_CONSTRAINT_NAME_LEN] = '\0';
			len = strlen(buf);
		}
	}

	return len;
}