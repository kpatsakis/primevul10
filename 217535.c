
static void kmem_cache_release(struct kobject *k)
{
	slab_kmem_cache_release(to_slab(k));