
static void sysfs_slab_remove(struct kmem_cache *s)
{
	if (slab_state < FULL)
		/*
		 * Sysfs has not been setup yet so no need to remove the
		 * cache from sysfs.
		 */
		return;

	kobject_get(&s->kobj);
	schedule_work(&s->kobj_remove_work);