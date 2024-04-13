static int validate_slab_node(struct kmem_cache *s,
		struct kmem_cache_node *n)
{
	unsigned long count = 0;
	struct page *page;
	unsigned long flags;

	spin_lock_irqsave(&n->list_lock, flags);

	list_for_each_entry(page, &n->partial, slab_list) {
		validate_slab(s, page);
		count++;
	}
	if (count != n->nr_partial)
		pr_err("SLUB %s: %ld partial slabs counted but counter=%ld\n",
		       s->name, count, n->nr_partial);

	if (!(s->flags & SLAB_STORE_USER))
		goto out;

	list_for_each_entry(page, &n->full, slab_list) {
		validate_slab(s, page);
		count++;
	}
	if (count != atomic_long_read(&n->nr_slabs))
		pr_err("SLUB: %s %ld slabs counted but counter=%ld\n",
		       s->name, count, atomic_long_read(&n->nr_slabs));

out:
	spin_unlock_irqrestore(&n->list_lock, flags);
	return count;
}