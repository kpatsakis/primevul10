static long do_splice_to(struct file *in, loff_t *ppos,
			 struct pipe_inode_info *pipe, size_t len,
			 unsigned int flags)
{
	loff_t isize, left;
	int ret;

	if (unlikely(!in->f_op || !in->f_op->splice_read))
		return -EINVAL;

	if (unlikely(!(in->f_mode & FMODE_READ)))
		return -EBADF;

	ret = rw_verify_area(READ, in, ppos, len);
	if (unlikely(ret < 0))
		return ret;

	isize = i_size_read(in->f_mapping->host);
	if (unlikely(*ppos >= isize))
		return 0;
	
	left = isize - *ppos;
	if (unlikely(left < len))
		len = left;

	return in->f_op->splice_read(in, ppos, pipe, len, flags);
}