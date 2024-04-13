void __kmemcg_cache_deactivate(struct kmem_cache *s)
{
	/*
	 * Disable empty slabs caching. Used to avoid pinning offline
	 * memory cgroups by kmem pages that can be freed.
	 */
	slub_set_cpu_partial(s, 0);
	s->min_partial = 0;
}