static ssize_t red_zone_store(struct kmem_cache *s,
				const char *buf, size_t length)
{
	if (any_slab_objects(s))
		return -EBUSY;

	s->flags &= ~SLAB_RED_ZONE;
	if (buf[0] == '1') {
		s->flags |= SLAB_RED_ZONE;
	}
	calculate_sizes(s, -1);
	return length;
}