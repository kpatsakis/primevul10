static ssize_t sanity_checks_store(struct kmem_cache *s,
				const char *buf, size_t length)
{
	s->flags &= ~SLAB_CONSISTENCY_CHECKS;
	if (buf[0] == '1') {
		s->flags &= ~__CMPXCHG_DOUBLE;
		s->flags |= SLAB_CONSISTENCY_CHECKS;
	}
	return length;
}