static unsigned long *get_map(struct kmem_cache *s, struct page *page)
{
	void *p;
	void *addr = page_address(page);

	VM_BUG_ON(!irqs_disabled());

	spin_lock(&object_map_lock);

	bitmap_zero(object_map, page->objects);

	for (p = page->freelist; p; p = get_freepointer(s, p))
		set_bit(slab_index(p, s, addr), object_map);

	return object_map;
}