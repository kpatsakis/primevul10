static ssize_t align_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", s->align);
}