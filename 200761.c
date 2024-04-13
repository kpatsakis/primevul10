generic_file_splice_write_nolock(struct pipe_inode_info *pipe, struct file *out,
				 loff_t *ppos, size_t len, unsigned int flags)
{
	struct address_space *mapping = out->f_mapping;
	struct inode *inode = mapping->host;
	ssize_t ret;
	int err;

	err = remove_suid(out->f_dentry);
	if (unlikely(err))
		return err;

	ret = __splice_from_pipe(pipe, out, ppos, len, flags, pipe_to_file);
	if (ret > 0) {
		*ppos += ret;

		/*
		 * If file or inode is SYNC and we actually wrote some data,
		 * sync it.
		 */
		if (unlikely((out->f_flags & O_SYNC) || IS_SYNC(inode))) {
			err = generic_osync_inode(inode, mapping,
						  OSYNC_METADATA|OSYNC_DATA);

			if (err)
				ret = err;
		}
	}

	return ret;
}