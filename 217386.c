void *fixup_red_left(struct kmem_cache *s, void *p)
{
	if (kmem_cache_debug(s) && s->flags & SLAB_RED_ZONE)
		p += s->red_left_pad;

	return p;
}