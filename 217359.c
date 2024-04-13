static bool shuffle_freelist(struct kmem_cache *s, struct page *page)
{
	void *start;
	void *cur;
	void *next;
	unsigned long idx, pos, page_limit, freelist_count;

	if (page->objects < 2 || !s->random_seq)
		return false;

	freelist_count = oo_objects(s->oo);
	pos = get_random_int() % freelist_count;

	page_limit = page->objects * s->size;
	start = fixup_red_left(s, page_address(page));

	/* First entry is used as the base of the freelist */
	cur = next_freelist_entry(s, page, &pos, start, page_limit,
				freelist_count);
	cur = setup_object(s, page, cur);
	page->freelist = cur;

	for (idx = 1; idx < page->objects; idx++) {
		next = next_freelist_entry(s, page, &pos, start, page_limit,
			freelist_count);
		next = setup_object(s, page, next);
		set_freepointer(s, cur, next);
		cur = next;
	}
	set_freepointer(s, cur, NULL);

	return true;
}