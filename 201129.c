static int ext4_link(struct dentry *old_dentry,
		     struct inode *dir, struct dentry *dentry)
{
	handle_t *handle;
	struct inode *inode = old_dentry->d_inode;
	int err, retries = 0;

	if (EXT4_DIR_LINK_MAX(inode))
		return -EMLINK;

	/*
	 * Return -ENOENT if we've raced with unlink and i_nlink is 0.  Doing
	 * otherwise has the potential to corrupt the orphan inode list.
	 */
	if (inode->i_nlink == 0)
		return -ENOENT;

retry:
	handle = ext4_journal_start(dir, EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
					EXT4_INDEX_EXTRA_TRANS_BLOCKS);
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	inode->i_ctime = ext4_current_time(inode);
	ext4_inc_count(handle, inode);
	atomic_inc(&inode->i_count);

	err = ext4_add_entry(handle, dentry, inode);
	if (!err) {
		ext4_mark_inode_dirty(handle, inode);
		d_instantiate(dentry, inode);
	} else {
		drop_nlink(inode);
		iput(inode);
	}
	ext4_journal_stop(handle);
	if (err == -ENOSPC && ext4_should_retry_alloc(dir->i_sb, &retries))
		goto retry;
	return err;
}