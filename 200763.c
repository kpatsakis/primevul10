ssize_t generic_file_splice_read(struct file *in, loff_t *ppos,
				 struct pipe_inode_info *pipe, size_t len,
				 unsigned int flags)
{
	ssize_t spliced;
	int ret;

	ret = 0;
	spliced = 0;

	while (len) {
		ret = __generic_file_splice_read(in, ppos, pipe, len, flags);

		if (ret < 0)
			break;
		else if (!ret) {
			if (spliced)
				break;
			if (flags & SPLICE_F_NONBLOCK) {
				ret = -EAGAIN;
				break;
			}
		}

		*ppos += ret;
		len -= ret;
		spliced += ret;
	}

	if (spliced)
		return spliced;

	return ret;
}