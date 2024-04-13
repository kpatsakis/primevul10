static ssize_t object_size_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", s->object_size);
}