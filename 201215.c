static void ext4_print_free_blocks(struct inode *inode)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	printk(KERN_EMERG "Total free blocks count %lld\n",
			ext4_count_free_blocks(inode->i_sb));
	printk(KERN_EMERG "Free/Dirty block details\n");
	printk(KERN_EMERG "free_blocks=%lld\n",
			(long long)percpu_counter_sum(&sbi->s_freeblocks_counter));
	printk(KERN_EMERG "dirty_blocks=%lld\n",
			(long long)percpu_counter_sum(&sbi->s_dirtyblocks_counter));
	printk(KERN_EMERG "Block reservation details\n");
	printk(KERN_EMERG "i_reserved_data_blocks=%u\n",
			EXT4_I(inode)->i_reserved_data_blocks);
	printk(KERN_EMERG "i_reserved_meta_blocks=%u\n",
			EXT4_I(inode)->i_reserved_meta_blocks);
	return;
}