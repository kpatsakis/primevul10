static void validate_slab(struct kmem_cache *s, struct page *page)
{
	void *p;
	void *addr = page_address(page);
	unsigned long *map;

	slab_lock(page);

	if (!check_slab(s, page) || !on_freelist(s, page, NULL))
		goto unlock;

	/* Now we know that a valid freelist exists */
	map = get_map(s, page);
	for_each_object(p, s, addr, page->objects) {
		u8 val = test_bit(slab_index(p, s, addr), map) ?
			 SLUB_RED_INACTIVE : SLUB_RED_ACTIVE;

		if (!check_object(s, page, p, val))
			break;
	}
	put_map(map);
unlock:
	slab_unlock(page);
}