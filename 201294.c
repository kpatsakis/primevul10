static struct inode *ext4_alloc_inode(struct super_block *sb)
{
	struct ext4_inode_info *ei;

	ei = kmem_cache_alloc(ext4_inode_cachep, GFP_NOFS);
	if (!ei)
		return NULL;
#ifdef CONFIG_EXT4_FS_POSIX_ACL
	ei->i_acl = EXT4_ACL_NOT_CACHED;
	ei->i_default_acl = EXT4_ACL_NOT_CACHED;
#endif
	ei->vfs_inode.i_version = 1;
	ei->vfs_inode.i_data.writeback_index = 0;
	memset(&ei->i_cached_extent, 0, sizeof(struct ext4_ext_cache));
	INIT_LIST_HEAD(&ei->i_prealloc_list);
	spin_lock_init(&ei->i_prealloc_lock);
	/*
	 * Note:  We can be called before EXT4_SB(sb)->s_journal is set,
	 * therefore it can be null here.  Don't check it, just initialize
	 * jinode.
	 */
	jbd2_journal_init_jbd_inode(&ei->jinode, &ei->vfs_inode);
	ei->i_reserved_data_blocks = 0;
	ei->i_reserved_meta_blocks = 0;
	ei->i_allocated_meta_blocks = 0;
	ei->i_delalloc_reserved_flag = 0;
	spin_lock_init(&(ei->i_block_reservation_lock));
	return &ei->vfs_inode;
}