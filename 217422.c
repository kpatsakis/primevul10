static void process_slab(struct loc_track *t, struct kmem_cache *s,
		struct page *page, enum track_item alloc)
{
	void *addr = page_address(page);
	void *p;
	unsigned long *map;

	map = get_map(s, page);
	for_each_object(p, s, addr, page->objects)
		if (!test_bit(slab_index(p, s, addr), map))
			add_location(t, s, get_track(s, p, alloc));
	put_map(map);
}