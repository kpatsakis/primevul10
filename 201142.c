int ext4_write_inode(struct inode *inode, int wait)
{
	if (current->flags & PF_MEMALLOC)
		return 0;

	if (ext4_journal_current_handle()) {
		jbd_debug(1, "called recursively, non-PF_MEMALLOC!\n");
		dump_stack();
		return -EIO;
	}

	if (!wait)
		return 0;

	return ext4_force_commit(inode->i_sb);
}