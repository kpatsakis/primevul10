static inline int alloc_consistency_checks(struct kmem_cache *s,
					struct page *page, void *object)
{
	if (!check_slab(s, page))
		return 0;

	if (!check_valid_pointer(s, page, object)) {
		object_err(s, page, object, "Freelist Pointer check fails");
		return 0;
	}

	if (!check_object(s, page, object, SLUB_RED_INACTIVE))
		return 0;

	return 1;
}