static __always_inline void maybe_wipe_obj_freeptr(struct kmem_cache *s,
						   void *obj)
{
	if (unlikely(slab_want_init_on_free(s)) && obj)
		memset((void *)((char *)obj + s->offset), 0, sizeof(void *));
}