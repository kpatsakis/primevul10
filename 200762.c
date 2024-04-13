long do_splice_direct(struct file *in, loff_t *ppos, struct file *out,
		      size_t len, unsigned int flags)
{
	struct pipe_inode_info *pipe;
	long ret, bytes;
	loff_t out_off;
	umode_t i_mode;
	int i;

	/*
	 * We require the input being a regular file, as we don't want to
	 * randomly drop data for eg socket -> socket splicing. Use the
	 * piped splicing for that!
	 */
	i_mode = in->f_dentry->d_inode->i_mode;
	if (unlikely(!S_ISREG(i_mode) && !S_ISBLK(i_mode)))
		return -EINVAL;

	/*
	 * neither in nor out is a pipe, setup an internal pipe attached to
	 * 'out' and transfer the wanted data from 'in' to 'out' through that
	 */
	pipe = current->splice_pipe;
	if (unlikely(!pipe)) {
		pipe = alloc_pipe_info(NULL);
		if (!pipe)
			return -ENOMEM;

		/*
		 * We don't have an immediate reader, but we'll read the stuff
		 * out of the pipe right after the splice_to_pipe(). So set
		 * PIPE_READERS appropriately.
		 */
		pipe->readers = 1;

		current->splice_pipe = pipe;
	}

	/*
	 * Do the splice.
	 */
	ret = 0;
	bytes = 0;
	out_off = 0;

	while (len) {
		size_t read_len, max_read_len;

		/*
		 * Do at most PIPE_BUFFERS pages worth of transfer:
		 */
		max_read_len = min(len, (size_t)(PIPE_BUFFERS*PAGE_SIZE));

		ret = do_splice_to(in, ppos, pipe, max_read_len, flags);
		if (unlikely(ret < 0))
			goto out_release;

		read_len = ret;

		/*
		 * NOTE: nonblocking mode only applies to the input. We
		 * must not do the output in nonblocking mode as then we
		 * could get stuck data in the internal pipe:
		 */
		ret = do_splice_from(pipe, out, &out_off, read_len,
				     flags & ~SPLICE_F_NONBLOCK);
		if (unlikely(ret < 0))
			goto out_release;

		bytes += ret;
		len -= ret;

		/*
		 * In nonblocking mode, if we got back a short read then
		 * that was due to either an IO error or due to the
		 * pagecache entry not being there. In the IO error case
		 * the _next_ splice attempt will produce a clean IO error
		 * return value (not a short read), so in both cases it's
		 * correct to break out of the loop here:
		 */
		if ((flags & SPLICE_F_NONBLOCK) && (read_len < max_read_len))
			break;
	}

	pipe->nrbufs = pipe->curbuf = 0;

	return bytes;

out_release:
	/*
	 * If we did an incomplete transfer we must release
	 * the pipe buffers in question:
	 */
	for (i = 0; i < PIPE_BUFFERS; i++) {
		struct pipe_buffer *buf = pipe->bufs + i;

		if (buf->ops) {
			buf->ops->release(pipe, buf);
			buf->ops = NULL;
		}
	}
	pipe->nrbufs = pipe->curbuf = 0;

	/*
	 * If we transferred some data, return the number of bytes:
	 */
	if (bytes > 0)
		return bytes;

	return ret;
}