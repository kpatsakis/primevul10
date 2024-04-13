static ssize_t order_show(struct kmem_cache *s, char *buf)
{
	return sprintf(buf, "%u\n", oo_order(s->oo));
}