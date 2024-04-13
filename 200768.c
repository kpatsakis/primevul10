static long do_vmsplice(struct file *file, const struct iovec __user *iov,
			unsigned long nr_segs, unsigned int flags)
{
	struct pipe_inode_info *pipe = file->f_dentry->d_inode->i_pipe;
	struct page *pages[PIPE_BUFFERS];
	struct partial_page partial[PIPE_BUFFERS];
	struct splice_pipe_desc spd = {
		.pages = pages,
		.partial = partial,
		.flags = flags,
		.ops = &user_page_pipe_buf_ops,
	};

	if (unlikely(!pipe))
		return -EBADF;
	if (unlikely(nr_segs > UIO_MAXIOV))
		return -EINVAL;
	else if (unlikely(!nr_segs))
		return 0;

	spd.nr_pages = get_iovec_page_array(iov, nr_segs, pages, partial,
					    flags & SPLICE_F_GIFT);
	if (spd.nr_pages <= 0)
		return spd.nr_pages;

	return splice_to_pipe(pipe, &spd);
}