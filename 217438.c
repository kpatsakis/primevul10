static inline unsigned int slab_index(void *p, struct kmem_cache *s, void *addr)
{
	return (kasan_reset_tag(p) - addr) / s->size;
}