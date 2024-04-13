static void destroy_inodecache(void)
{
	kmem_cache_destroy(shmem_inode_cachep);
}