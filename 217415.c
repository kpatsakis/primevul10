int __kmem_cache_create(struct kmem_cache *s, slab_flags_t flags)
{
	int err;

	err = kmem_cache_open(s, flags);
	if (err)
		return err;

	/* Mutex is not taken during early boot */
	if (slab_state <= UP)
		return 0;

	memcg_propagate_slab_attrs(s);
	err = sysfs_slab_add(s);
	if (err)
		__kmem_cache_release(s);

	return err;
}