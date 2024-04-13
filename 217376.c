static inline bool slab_free_freelist_hook(struct kmem_cache *s,
					   void **head, void **tail)
{

	void *object;
	void *next = *head;
	void *old_tail = *tail ? *tail : *head;
	int rsize;

	/* Head and tail of the reconstructed freelist */
	*head = NULL;
	*tail = NULL;

	do {
		object = next;
		next = get_freepointer(s, object);

		if (slab_want_init_on_free(s)) {
			/*
			 * Clear the object and the metadata, but don't touch
			 * the redzone.
			 */
			memset(object, 0, s->object_size);
			rsize = (s->flags & SLAB_RED_ZONE) ? s->red_left_pad
							   : 0;
			memset((char *)object + s->inuse, 0,
			       s->size - s->inuse - rsize);

		}
		/* If object's reuse doesn't have to be delayed */
		if (!slab_free_hook(s, object)) {
			/* Move object to the new freelist */
			set_freepointer(s, object, *head);
			*head = object;
			if (!*tail)
				*tail = object;
		}
	} while (object != old_tail);

	if (*head == *tail)
		*tail = NULL;

	return *head != NULL;
}