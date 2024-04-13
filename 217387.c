static ssize_t order_store(struct kmem_cache *s,
				const char *buf, size_t length)
{
	unsigned int order;
	int err;

	err = kstrtouint(buf, 10, &order);
	if (err)
		return err;

	if (order > slub_max_order || order < slub_min_order)
		return -EINVAL;

	calculate_sizes(s, order);
	return length;
}