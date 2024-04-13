static noinline int free_debug_processing(
	struct kmem_cache *s, struct page *page,
	void *head, void *tail, int bulk_cnt,
	unsigned long addr)
{
	struct kmem_cache_node *n = get_node(s, page_to_nid(page));
	void *object = head;
	int cnt = 0;
	unsigned long uninitialized_var(flags);
	int ret = 0;

	spin_lock_irqsave(&n->list_lock, flags);
	slab_lock(page);

	if (s->flags & SLAB_CONSISTENCY_CHECKS) {
		if (!check_slab(s, page))
			goto out;
	}

next_object:
	cnt++;

	if (s->flags & SLAB_CONSISTENCY_CHECKS) {
		if (!free_consistency_checks(s, page, object, addr))
			goto out;
	}

	if (s->flags & SLAB_STORE_USER)
		set_track(s, object, TRACK_FREE, addr);
	trace(s, page, object, 0);
	/* Freepointer not overwritten by init_object(), SLAB_POISON moved it */
	init_object(s, object, SLUB_RED_INACTIVE);

	/* Reached end of constructed freelist yet? */
	if (object != tail) {
		object = get_freepointer(s, object);
		goto next_object;
	}
	ret = 1;

out:
	if (cnt != bulk_cnt)
		slab_err(s, page, "Bulk freelist count(%d) invalid(%d)\n",
			 bulk_cnt, cnt);

	slab_unlock(page);
	spin_unlock_irqrestore(&n->list_lock, flags);
	if (!ret)
		slab_fix(s, "Object at 0x%p not freed", object);
	return ret;
}