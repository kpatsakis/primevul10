static int shmem_fill_super(struct super_block *sb,
			    void *data, int silent)
{
	struct inode *inode;
	struct dentry *root;
	int mode   = S_IRWXUGO | S_ISVTX;
	uid_t uid = current->fsuid;
	gid_t gid = current->fsgid;
	int err = -ENOMEM;
	struct shmem_sb_info *sbinfo;
	unsigned long blocks = 0;
	unsigned long inodes = 0;
	int policy = MPOL_DEFAULT;
	nodemask_t policy_nodes = node_states[N_HIGH_MEMORY];

#ifdef CONFIG_TMPFS
	/*
	 * Per default we only allow half of the physical ram per
	 * tmpfs instance, limiting inodes to one per page of lowmem;
	 * but the internal instance is left unlimited.
	 */
	if (!(sb->s_flags & MS_NOUSER)) {
		blocks = totalram_pages / 2;
		inodes = totalram_pages - totalhigh_pages;
		if (inodes > blocks)
			inodes = blocks;
		if (shmem_parse_options(data, &mode, &uid, &gid, &blocks,
					&inodes, &policy, &policy_nodes))
			return -EINVAL;
	}
	sb->s_export_op = &shmem_export_ops;
#else
	sb->s_flags |= MS_NOUSER;
#endif

	/* Round up to L1_CACHE_BYTES to resist false sharing */
	sbinfo = kmalloc(max((int)sizeof(struct shmem_sb_info),
				L1_CACHE_BYTES), GFP_KERNEL);
	if (!sbinfo)
		return -ENOMEM;

	spin_lock_init(&sbinfo->stat_lock);
	sbinfo->max_blocks = blocks;
	sbinfo->free_blocks = blocks;
	sbinfo->max_inodes = inodes;
	sbinfo->free_inodes = inodes;
	sbinfo->policy = policy;
	sbinfo->policy_nodes = policy_nodes;

	sb->s_fs_info = sbinfo;
	sb->s_maxbytes = SHMEM_MAX_BYTES;
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = TMPFS_MAGIC;
	sb->s_op = &shmem_ops;
	sb->s_time_gran = 1;
#ifdef CONFIG_TMPFS_POSIX_ACL
	sb->s_xattr = shmem_xattr_handlers;
	sb->s_flags |= MS_POSIXACL;
#endif

	inode = shmem_get_inode(sb, S_IFDIR | mode, 0);
	if (!inode)
		goto failed;
	inode->i_uid = uid;
	inode->i_gid = gid;
	root = d_alloc_root(inode);
	if (!root)
		goto failed_iput;
	sb->s_root = root;
	return 0;

failed_iput:
	iput(inode);
failed:
	shmem_put_super(sb);
	return err;
}