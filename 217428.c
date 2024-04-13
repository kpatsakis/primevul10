static ssize_t remote_node_defrag_ratio_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", s->remote_node_defrag_ratio / 10);
}