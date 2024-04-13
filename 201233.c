static void ext4_clear_blocks(handle_t *handle, struct inode *inode,
		struct buffer_head *bh, ext4_fsblk_t block_to_free,
		unsigned long count, __le32 *first, __le32 *last)
{
	__le32 *p;
	if (try_to_extend_transaction(handle, inode)) {
		if (bh) {
			BUFFER_TRACE(bh, "call ext4_handle_dirty_metadata");
			ext4_handle_dirty_metadata(handle, inode, bh);
		}
		ext4_mark_inode_dirty(handle, inode);
		ext4_journal_test_restart(handle, inode);
		if (bh) {
			BUFFER_TRACE(bh, "retaking write access");
			ext4_journal_get_write_access(handle, bh);
		}
	}

	/*
	 * Any buffers which are on the journal will be in memory. We find
	 * them on the hash table so jbd2_journal_revoke() will run jbd2_journal_forget()
	 * on them.  We've already detached each block from the file, so
	 * bforget() in jbd2_journal_forget() should be safe.
	 *
	 * AKPM: turn on bforget in jbd2_journal_forget()!!!
	 */
	for (p = first; p < last; p++) {
		u32 nr = le32_to_cpu(*p);
		if (nr) {
			struct buffer_head *tbh;

			*p = 0;
			tbh = sb_find_get_block(inode->i_sb, nr);
			ext4_forget(handle, 0, inode, tbh, nr);
		}
	}

	ext4_free_blocks(handle, inode, block_to_free, count, 0);
}