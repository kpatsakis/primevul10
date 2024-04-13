void __check_heap_object(const void *ptr, unsigned long n, struct page *page,
			 bool to_user)
{
	struct kmem_cache *s;
	unsigned int offset;
	size_t object_size;

	ptr = kasan_reset_tag(ptr);

	/* Find object and usable object size. */
	s = page->slab_cache;

	/* Reject impossible pointers. */
	if (ptr < page_address(page))
		usercopy_abort("SLUB object not in SLUB page?!", NULL,
			       to_user, 0, n);

	/* Find offset within object. */
	offset = (ptr - page_address(page)) % s->size;

	/* Adjust for redzone and reject if within the redzone. */
	if (kmem_cache_debug(s) && s->flags & SLAB_RED_ZONE) {
		if (offset < s->red_left_pad)
			usercopy_abort("SLUB object in left red zone",
				       s->name, to_user, offset, n);
		offset -= s->red_left_pad;
	}

	/* Allow address range falling entirely within usercopy region. */
	if (offset >= s->useroffset &&
	    offset - s->useroffset <= s->usersize &&
	    n <= s->useroffset - offset + s->usersize)
		return;

	/*
	 * If the copy is still within the allocated object, produce
	 * a warning instead of rejecting the copy. This is intended
	 * to be a temporary method to find any missing usercopy
	 * whitelists.
	 */
	object_size = slab_ksize(s);
	if (usercopy_fallback &&
	    offset <= object_size && n <= object_size - offset) {
		usercopy_warn("SLUB object", s->name, to_user, offset, n);
		return;
	}

	usercopy_abort("SLUB object", s->name, to_user, offset, n);
}