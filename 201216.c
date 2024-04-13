int __ext4_write_dirty_metadata(struct inode *inode, struct buffer_head *bh)
{
	int err = 0;

	mark_buffer_dirty(bh);
	if (inode && inode_needs_sync(inode)) {
		sync_dirty_buffer(bh);
		if (buffer_req(bh) && !buffer_uptodate(bh)) {
			ext4_error(inode->i_sb, __func__,
				   "IO error syncing inode, "
				   "inode=%lu, block=%llu",
				   inode->i_ino,
				   (unsigned long long)bh->b_blocknr);
			err = -EIO;
		}
	}
	return err;
}