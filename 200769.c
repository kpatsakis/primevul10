static long do_tee(struct file *in, struct file *out, size_t len,
		   unsigned int flags)
{
	struct pipe_inode_info *ipipe = in->f_dentry->d_inode->i_pipe;
	struct pipe_inode_info *opipe = out->f_dentry->d_inode->i_pipe;
	int ret = -EINVAL;

	/*
	 * Duplicate the contents of ipipe to opipe without actually
	 * copying the data.
	 */
	if (ipipe && opipe && ipipe != opipe) {
		/*
		 * Keep going, unless we encounter an error. The ipipe/opipe
		 * ordering doesn't really matter.
		 */
		ret = link_ipipe_prep(ipipe, flags);
		if (!ret) {
			ret = link_opipe_prep(opipe, flags);
			if (!ret) {
				ret = link_pipe(ipipe, opipe, len, flags);
				if (!ret && (flags & SPLICE_F_NONBLOCK))
					ret = -EAGAIN;
			}
		}
	}

	return ret;
}