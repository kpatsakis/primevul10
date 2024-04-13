void ___cache_free(struct kmem_cache *cache, void *x, unsigned long addr)
{
	do_slab_free(cache, virt_to_head_page(x), x, NULL, 1, addr);
}