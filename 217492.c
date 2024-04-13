static void list_slab_objects(struct kmem_cache *s, struct page *page,
							const char *text)
{
#ifdef CONFIG_SLUB_DEBUG
	void *addr = page_address(page);
	void *p;
	unsigned long *map;

	slab_err(s, page, text, s->name);
	slab_lock(page);

	map = get_map(s, page);
	for_each_object(p, s, addr, page->objects) {

		if (!test_bit(slab_index(p, s, addr), map)) {
			pr_err("INFO: Object 0x%p @offset=%tu\n", p, p - addr);
			print_tracking(s, p);
		}
	}
	put_map(map);

	slab_unlock(page);
#endif
}