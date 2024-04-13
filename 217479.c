void kfree(const void *x)
{
	struct page *page;
	void *object = (void *)x;

	trace_kfree(_RET_IP_, x);

	if (unlikely(ZERO_OR_NULL_PTR(x)))
		return;

	page = virt_to_head_page(x);
	if (unlikely(!PageSlab(page))) {
		unsigned int order = compound_order(page);

		BUG_ON(!PageCompound(page));
		kfree_hook(object);
		mod_node_page_state(page_pgdat(page), NR_SLAB_UNRECLAIMABLE,
				    -(1 << order));
		__free_pages(page, order);
		return;
	}
	slab_free(page->slab_cache, page, object, NULL, 1, _RET_IP_);
}