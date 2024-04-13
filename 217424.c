static void init_object(struct kmem_cache *s, void *object, u8 val)
{
	u8 *p = object;

	if (s->flags & SLAB_RED_ZONE)
		memset(p - s->red_left_pad, val, s->red_left_pad);

	if (s->flags & __OBJECT_POISON) {
		memset(p, POISON_FREE, s->object_size - 1);
		p[s->object_size - 1] = POISON_END;
	}

	if (s->flags & SLAB_RED_ZONE)
		memset(p + s->object_size, val, s->inuse - s->object_size);
}