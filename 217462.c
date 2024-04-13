static void __free_slab(struct kmem_cache *s, struct page *page)
{
	int order = compound_order(page);
	int pages = 1 << order;

	if (s->flags & SLAB_CONSISTENCY_CHECKS) {
		void *p;

		slab_pad_check(s, page);
		for_each_object(p, s, page_address(page),
						page->objects)
			check_object(s, page, p, SLUB_RED_INACTIVE);
	}

	__ClearPageSlabPfmemalloc(page);
	__ClearPageSlab(page);

	page->mapping = NULL;
	if (current->reclaim_state)
		current->reclaim_state->reclaimed_slab += pages;
	uncharge_slab_page(page, order, s);
	__free_pages(page, order);
}