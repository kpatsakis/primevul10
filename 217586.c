int __kmem_cache_shutdown(struct kmem_cache *s)
{
	int node;
	struct kmem_cache_node *n;

	flush_all(s);
	/* Attempt to free all objects */
	for_each_kmem_cache_node(s, node, n) {
		free_partial(s, n);
		if (n->nr_partial || slabs_node(s, node))
			return 1;
	}
	sysfs_slab_remove(s);
	return 0;
}