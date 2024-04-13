static inline void remove_partial(struct kmem_cache_node *n,
					struct page *page)
{
	lockdep_assert_held(&n->list_lock);
	list_del(&page->slab_list);
	n->nr_partial--;
}