void *kmem_cache_alloc_node_trace(struct kmem_cache *s,
				    gfp_t gfpflags,
				    int node, size_t size)
{
	void *ret = slab_alloc_node(s, gfpflags, node, _RET_IP_);

	trace_kmalloc_node(_RET_IP_, ret,
			   size, s->size, gfpflags, node);

	ret = kasan_kmalloc(s, ret, size, gfpflags);
	return ret;
}