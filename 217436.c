void kmem_cache_free_bulk(struct kmem_cache *s, size_t size, void **p)
{
	if (WARN_ON(!size))
		return;

	do {
		struct detached_freelist df;

		size = build_detached_freelist(s, size, p, &df);
		if (!df.page)
			continue;

		slab_free(df.s, df.page, df.freelist, df.tail, df.cnt,_RET_IP_);
	} while (likely(size));
}