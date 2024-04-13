static int create_constraint_attribute(int id, const char *name,
				int mode,
				struct device_attribute *dev_attr,
				ssize_t (*show)(struct device *,
					struct device_attribute *, char *),
				ssize_t (*store)(struct device *,
					struct device_attribute *,
				const char *, size_t)
				)
{

	dev_attr->attr.name = kasprintf(GFP_KERNEL, "constraint_%d_%s",
								id, name);
	if (!dev_attr->attr.name)
		return -ENOMEM;
	dev_attr->attr.mode = mode;
	dev_attr->show = show;
	dev_attr->store = store;

	return 0;
}