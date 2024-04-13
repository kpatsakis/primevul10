static void free_partial(struct kmem_cache *s, struct kmem_cache_node *n)
{
	LIST_HEAD(discard);
	struct page *page, *h;

	BUG_ON(irqs_disabled());
	spin_lock_irq(&n->list_lock);
	list_for_each_entry_safe(page, h, &n->partial, slab_list) {
		if (!page->inuse) {
			remove_partial(n, page);
			list_add(&page->slab_list, &discard);
		} else {
			list_slab_objects(s, page,
			"Objects remaining in %s on __kmem_cache_shutdown()");
		}
	}
	spin_unlock_irq(&n->list_lock);

	list_for_each_entry_safe(page, h, &discard, slab_list)
		discard_slab(s, page);
}