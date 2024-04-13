static int any_slab_objects(struct kmem_cache *s)
{
	int node;
	struct kmem_cache_node *n;

	for_each_kmem_cache_node(s, node, n)
		if (atomic_long_read(&n->total_objects))
			return 1;

	return 0;
}