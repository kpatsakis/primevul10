static ssize_t cpu_partial_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", slub_cpu_partial(s));
}