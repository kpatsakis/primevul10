void setup_page_debug(struct kmem_cache *s, struct page *page, void *addr)
{
	if (!(s->flags & SLAB_POISON))
		return;

	metadata_access_enable();
	memset(addr, POISON_INUSE, page_size(page));
	metadata_access_disable();
}