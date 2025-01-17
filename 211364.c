int ext4_group_add(struct super_block *sb, struct ext4_new_group_data *input)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	int reserved_gdb = ext4_bg_has_super(sb, input->group) ?
		le16_to_cpu(es->s_reserved_gdt_blocks) : 0;
	struct buffer_head *primary = NULL;
	struct ext4_group_desc *gdp;
	struct inode *inode = NULL;
	handle_t *handle;
	int gdb_off, gdb_num;
	int num_grp_locked = 0;
	int err, err2;

	gdb_num = input->group / EXT4_DESC_PER_BLOCK(sb);
	gdb_off = input->group % EXT4_DESC_PER_BLOCK(sb);

	if (gdb_off == 0 && !EXT4_HAS_RO_COMPAT_FEATURE(sb,
					EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER)) {
		ext4_warning(sb, __func__,
			     "Can't resize non-sparse filesystem further");
		return -EPERM;
	}

	if (ext4_blocks_count(es) + input->blocks_count <
	    ext4_blocks_count(es)) {
		ext4_warning(sb, __func__, "blocks_count overflow");
		return -EINVAL;
	}

	if (le32_to_cpu(es->s_inodes_count) + EXT4_INODES_PER_GROUP(sb) <
	    le32_to_cpu(es->s_inodes_count)) {
		ext4_warning(sb, __func__, "inodes_count overflow");
		return -EINVAL;
	}

	if (reserved_gdb || gdb_off == 0) {
		if (!EXT4_HAS_COMPAT_FEATURE(sb,
					     EXT4_FEATURE_COMPAT_RESIZE_INODE)
		    || !le16_to_cpu(es->s_reserved_gdt_blocks)) {
			ext4_warning(sb, __func__,
				     "No reserved GDT blocks, can't resize");
			return -EPERM;
		}
		inode = ext4_iget(sb, EXT4_RESIZE_INO);
		if (IS_ERR(inode)) {
			ext4_warning(sb, __func__,
				     "Error opening resize inode");
			return PTR_ERR(inode);
		}
	}


	if ((err = verify_group_input(sb, input)))
		goto exit_put;

	if ((err = setup_new_group_blocks(sb, input)))
		goto exit_put;

	/*
	 * We will always be modifying at least the superblock and a GDT
	 * block.  If we are adding a group past the last current GDT block,
	 * we will also modify the inode and the dindirect block.  If we
	 * are adding a group with superblock/GDT backups  we will also
	 * modify each of the reserved GDT dindirect blocks.
	 */
	handle = ext4_journal_start_sb(sb,
				       ext4_bg_has_super(sb, input->group) ?
				       3 + reserved_gdb : 4);
	if (IS_ERR(handle)) {
		err = PTR_ERR(handle);
		goto exit_put;
	}

	lock_super(sb);
	if (input->group != sbi->s_groups_count) {
		ext4_warning(sb, __func__,
			     "multiple resizers run on filesystem!");
		err = -EBUSY;
		goto exit_journal;
	}

	if ((err = ext4_journal_get_write_access(handle, sbi->s_sbh)))
		goto exit_journal;

        /*
         * We will only either add reserved group blocks to a backup group
         * or remove reserved blocks for the first group in a new group block.
         * Doing both would be mean more complex code, and sane people don't
         * use non-sparse filesystems anymore.  This is already checked above.
         */
	if (gdb_off) {
		primary = sbi->s_group_desc[gdb_num];
		if ((err = ext4_journal_get_write_access(handle, primary)))
			goto exit_journal;

		if (reserved_gdb && ext4_bg_num_gdb(sb, input->group) &&
		    (err = reserve_backup_gdb(handle, inode, input)))
			goto exit_journal;
	} else if ((err = add_new_gdb(handle, inode, input, &primary)))
		goto exit_journal;

        /*
         * OK, now we've set up the new group.  Time to make it active.
         *
         * Current kernels don't lock all allocations via lock_super(),
         * so we have to be safe wrt. concurrent accesses the group
         * data.  So we need to be careful to set all of the relevant
         * group descriptor data etc. *before* we enable the group.
         *
         * The key field here is sbi->s_groups_count: as long as
         * that retains its old value, nobody is going to access the new
         * group.
         *
         * So first we update all the descriptor metadata for the new
         * group; then we update the total disk blocks count; then we
         * update the groups count to enable the group; then finally we
         * update the free space counts so that the system can start
         * using the new disk blocks.
         */

	num_grp_locked = ext4_mb_get_buddy_cache_lock(sb, input->group);
	/* Update group descriptor block for new group */
	gdp = (struct ext4_group_desc *)((char *)primary->b_data +
					 gdb_off * EXT4_DESC_SIZE(sb));

	ext4_block_bitmap_set(sb, gdp, input->block_bitmap); /* LV FIXME */
	ext4_inode_bitmap_set(sb, gdp, input->inode_bitmap); /* LV FIXME */
	ext4_inode_table_set(sb, gdp, input->inode_table); /* LV FIXME */
	ext4_free_blks_set(sb, gdp, input->free_blocks_count);
	ext4_free_inodes_set(sb, gdp, EXT4_INODES_PER_GROUP(sb));
	gdp->bg_flags |= cpu_to_le16(EXT4_BG_INODE_ZEROED);
	gdp->bg_checksum = ext4_group_desc_csum(sbi, input->group, gdp);

	/*
	 * We can allocate memory for mb_alloc based on the new group
	 * descriptor
	 */
	err = ext4_mb_add_groupinfo(sb, input->group, gdp);
	if (err) {
		ext4_mb_put_buddy_cache_lock(sb, input->group, num_grp_locked);
		goto exit_journal;
	}

	/*
	 * Make the new blocks and inodes valid next.  We do this before
	 * increasing the group count so that once the group is enabled,
	 * all of its blocks and inodes are already valid.
	 *
	 * We always allocate group-by-group, then block-by-block or
	 * inode-by-inode within a group, so enabling these
	 * blocks/inodes before the group is live won't actually let us
	 * allocate the new space yet.
	 */
	ext4_blocks_count_set(es, ext4_blocks_count(es) +
		input->blocks_count);
	le32_add_cpu(&es->s_inodes_count, EXT4_INODES_PER_GROUP(sb));

	/*
	 * We need to protect s_groups_count against other CPUs seeing
	 * inconsistent state in the superblock.
	 *
	 * The precise rules we use are:
	 *
	 * * Writers of s_groups_count *must* hold lock_super
	 * AND
	 * * Writers must perform a smp_wmb() after updating all dependent
	 *   data and before modifying the groups count
	 *
	 * * Readers must hold lock_super() over the access
	 * OR
	 * * Readers must perform an smp_rmb() after reading the groups count
	 *   and before reading any dependent data.
	 *
	 * NB. These rules can be relaxed when checking the group count
	 * while freeing data, as we can only allocate from a block
	 * group after serialising against the group count, and we can
	 * only then free after serialising in turn against that
	 * allocation.
	 */
	smp_wmb();

	/* Update the global fs size fields */
	sbi->s_groups_count++;
	ext4_mb_put_buddy_cache_lock(sb, input->group, num_grp_locked);

	ext4_handle_dirty_metadata(handle, NULL, primary);

	/* Update the reserved block counts only once the new group is
	 * active. */
	ext4_r_blocks_count_set(es, ext4_r_blocks_count(es) +
		input->reserved_blocks);

	/* Update the free space counts */
	percpu_counter_add(&sbi->s_freeblocks_counter,
			   input->free_blocks_count);
	percpu_counter_add(&sbi->s_freeinodes_counter,
			   EXT4_INODES_PER_GROUP(sb));

	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_FLEX_BG)) {
		ext4_group_t flex_group;
		flex_group = ext4_flex_group(sbi, input->group);
		sbi->s_flex_groups[flex_group].free_blocks +=
			input->free_blocks_count;
		sbi->s_flex_groups[flex_group].free_inodes +=
			EXT4_INODES_PER_GROUP(sb);
	}

	ext4_handle_dirty_metadata(handle, NULL, sbi->s_sbh);
	sb->s_dirt = 1;

exit_journal:
	unlock_super(sb);
	if ((err2 = ext4_journal_stop(handle)) && !err)
		err = err2;
	if (!err) {
		update_backups(sb, sbi->s_sbh->b_blocknr, (char *)es,
			       sizeof(struct ext4_super_block));
		update_backups(sb, primary->b_blocknr, primary->b_data,
			       primary->b_size);
	}
exit_put:
	iput(inode);
	return err;
} /* ext4_group_add */