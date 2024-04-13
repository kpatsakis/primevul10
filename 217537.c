
void sysfs_slab_release(struct kmem_cache *s)
{
	if (slab_state >= FULL)
		kobject_put(&s->kobj);