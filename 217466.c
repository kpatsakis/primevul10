
void sysfs_slab_unlink(struct kmem_cache *s)
{
	if (slab_state >= FULL)
		kobject_del(&s->kobj);