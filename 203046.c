int agp_unbind_memory(struct agp_memory *curr)
{
	int ret_val;

	if (curr == NULL)
		return -EINVAL;

	if (!curr->is_bound) {
		printk(KERN_INFO PFX "memory %p was not bound!\n", curr);
		return -EINVAL;
	}

	ret_val = curr->bridge->driver->remove_memory(curr, curr->pg_start, curr->type);

	if (ret_val != 0)
		return ret_val;

	curr->is_bound = false;
	curr->pg_start = 0;
	spin_lock(&curr->bridge->mapped_lock);
	list_del(&curr->mapped_list);
	spin_unlock(&curr->bridge->mapped_lock);
	return 0;
}