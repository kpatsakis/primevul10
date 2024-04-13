static int slab_mem_going_offline_callback(void *arg)
{
	struct kmem_cache *s;

	mutex_lock(&slab_mutex);
	list_for_each_entry(s, &slab_caches, list)
		__kmem_cache_shrink(s);
	mutex_unlock(&slab_mutex);

	return 0;
}