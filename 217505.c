static ssize_t usersize_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", s->usersize);
}