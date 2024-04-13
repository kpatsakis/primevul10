int ext4_mark_iloc_dirty(handle_t *handle,
		struct inode *inode, struct ext4_iloc *iloc)
{
	int err = 0;

	if (test_opt(inode->i_sb, I_VERSION))
		inode_inc_iversion(inode);

	/* the do_update_inode consumes one bh->b_count */
	get_bh(iloc->bh);

	/* ext4_do_update_inode() does jbd2_journal_dirty_metadata */
	err = ext4_do_update_inode(handle, inode, iloc);
	put_bh(iloc->bh);
	return err;
}