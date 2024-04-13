static int ext4_ordered_write_end(struct file *file,
				struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct page *page, void *fsdata)
{
	handle_t *handle = ext4_journal_current_handle();
	struct inode *inode = mapping->host;
	int ret = 0, ret2;

	trace_mark(ext4_ordered_write_end,
		   "dev %s ino %lu pos %llu len %u copied %u",
		   inode->i_sb->s_id, inode->i_ino,
		   (unsigned long long) pos, len, copied);
	ret = ext4_jbd2_file_inode(handle, inode);

	if (ret == 0) {
		loff_t new_i_size;

		new_i_size = pos + copied;
		if (new_i_size > EXT4_I(inode)->i_disksize) {
			ext4_update_i_disksize(inode, new_i_size);
			/* We need to mark inode dirty even if
			 * new_i_size is less that inode->i_size
			 * bu greater than i_disksize.(hint delalloc)
			 */
			ext4_mark_inode_dirty(handle, inode);
		}

		ret2 = generic_write_end(file, mapping, pos, len, copied,
							page, fsdata);
		copied = ret2;
		if (ret2 < 0)
			ret = ret2;
	}
	ret2 = ext4_journal_stop(handle);
	if (!ret)
		ret = ret2;

	return ret ? ret : copied;
}