static ssize_t trace_store(struct kmem_cache *s, const char *buf,
							size_t length)
{
	/*
	 * Tracing a merged cache is going to give confusing results
	 * as well as cause other issues like converting a mergeable
	 * cache into an umergeable one.
	 */
	if (s->refcount > 1)
		return -EINVAL;

	s->flags &= ~SLAB_TRACE;
	if (buf[0] == '1') {
		s->flags &= ~__CMPXCHG_DOUBLE;
		s->flags |= SLAB_TRACE;
	}
	return length;
}