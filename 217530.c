static unsigned long count_partial(struct kmem_cache_node *n,
					int (*get_count)(struct page *))
{
	unsigned long flags;
	unsigned long x = 0;
	struct page *page;

	spin_lock_irqsave(&n->list_lock, flags);
	list_for_each_entry(page, &n->partial, slab_list)
		x += get_count(page);
	spin_unlock_irqrestore(&n->list_lock, flags);
	return x;
}