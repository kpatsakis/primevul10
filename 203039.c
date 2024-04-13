int agp_bind_memory(struct agp_memory *curr, off_t pg_start)
{
	int ret_val;

	if (curr == NULL)
		return -EINVAL;

	if (curr->is_bound) {
		printk(KERN_INFO PFX "memory %p is already bound!\n", curr);
		return -EINVAL;
	}
	if (!curr->is_flushed) {
		curr->bridge->driver->cache_flush();
		curr->is_flushed = true;
	}

	ret_val = curr->bridge->driver->insert_memory(curr, pg_start, curr->type);

	if (ret_val != 0)
		return ret_val;

	curr->is_bound = true;
	curr->pg_start = pg_start;
	spin_lock(&agp_bridge->mapped_lock);
	list_add(&curr->mapped_list, &agp_bridge->mapped_list);
	spin_unlock(&agp_bridge->mapped_lock);

	return 0;
}