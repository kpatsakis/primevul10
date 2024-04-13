static inline int free_consistency_checks(struct kmem_cache *s,
		struct page *page, void *object, unsigned long addr)
{
	if (!check_valid_pointer(s, page, object)) {
		slab_err(s, page, "Invalid object pointer 0x%p", object);
		return 0;
	}

	if (on_freelist(s, page, object)) {
		object_err(s, page, object, "Object already free");
		return 0;
	}

	if (!check_object(s, page, object, SLUB_RED_ACTIVE))
		return 0;

	if (unlikely(s != page->slab_cache)) {
		if (!PageSlab(page)) {
			slab_err(s, page, "Attempt to free object(0x%p) outside of slab",
				 object);
		} else if (!page->slab_cache) {
			pr_err("SLUB <none>: no slab for object 0x%p.\n",
			       object);
			dump_stack();
		} else
			object_err(s, page, object,
					"page slab pointer corrupt.");
		return 0;
	}
	return 1;
}