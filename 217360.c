static void *kmalloc_large_node(size_t size, gfp_t flags, int node)
{
	struct page *page;
	void *ptr = NULL;
	unsigned int order = get_order(size);

	flags |= __GFP_COMP;
	page = alloc_pages_node(node, flags, order);
	if (page) {
		ptr = page_address(page);
		mod_node_page_state(page_pgdat(page), NR_SLAB_UNRECLAIMABLE,
				    1 << order);
	}

	return kmalloc_large_node_hook(ptr, size, flags);
}