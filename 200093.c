static void init_once(struct kmem_cache *cachep, void *foo)
{
	struct shmem_inode_info *p = (struct shmem_inode_info *) foo;

	inode_init_once(&p->vfs_inode);
#ifdef CONFIG_TMPFS_POSIX_ACL
	p->i_acl = NULL;
	p->i_default_acl = NULL;
#endif
}