static __always_inline void slab_free(struct kmem_cache *s, struct page *page,
				      void *head, void *tail, int cnt,
				      unsigned long addr)
{
	/*
	 * With KASAN enabled slab_free_freelist_hook modifies the freelist
	 * to remove objects, whose reuse must be delayed.
	 */
	if (slab_free_freelist_hook(s, &head, &tail))
		do_slab_free(s, page, head, tail, cnt, addr);
}