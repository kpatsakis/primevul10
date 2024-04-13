static int ext4_journal_test_restart(handle_t *handle, struct inode *inode)
{
	BUG_ON(EXT4_JOURNAL(inode) == NULL);
	jbd_debug(2, "restarting handle %p\n", handle);
	return ext4_journal_restart(handle, blocks_for_truncate(inode));
}