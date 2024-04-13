static ssize_t slab_size_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", s->size);
}