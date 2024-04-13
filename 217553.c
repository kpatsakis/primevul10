static ssize_t failslab_store(struct kmem_cache *s, const char *buf,
							size_t length)
{
	if (s->refcount > 1)
		return -EINVAL;

	s->flags &= ~SLAB_FAILSLAB;
	if (buf[0] == '1')
		s->flags |= SLAB_FAILSLAB;
	return length;
}