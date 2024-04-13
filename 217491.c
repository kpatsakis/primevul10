
static inline struct kset *cache_kset(struct kmem_cache *s)
{
#ifdef CONFIG_MEMCG
	if (!is_root_cache(s))
		return s->memcg_params.root_cache->memcg_kset;
#endif
	return slab_kset;