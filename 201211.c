void ext4_dirty_inode(struct inode *inode)
{
	handle_t *current_handle = ext4_journal_current_handle();
	handle_t *handle;

	if (!ext4_handle_valid(current_handle)) {
		ext4_mark_inode_dirty(current_handle, inode);
		return;
	}

	handle = ext4_journal_start(inode, 2);
	if (IS_ERR(handle))
		goto out;
	if (current_handle &&
		current_handle->h_transaction != handle->h_transaction) {
		/* This task has a transaction open against a different fs */
		printk(KERN_EMERG "%s: transactions do not match!\n",
		       __func__);
	} else {
		jbd_debug(5, "marking dirty.  outer handle=%p\n",
				current_handle);
		ext4_mark_inode_dirty(handle, inode);
	}
	ext4_journal_stop(handle);
out:
	return;
}