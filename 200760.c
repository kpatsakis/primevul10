static long do_splice(struct file *in, loff_t __user *off_in,
		      struct file *out, loff_t __user *off_out,
		      size_t len, unsigned int flags)
{
	struct pipe_inode_info *pipe;
	loff_t offset, *off;
	long ret;

	pipe = in->f_dentry->d_inode->i_pipe;
	if (pipe) {
		if (off_in)
			return -ESPIPE;
		if (off_out) {
			if (out->f_op->llseek == no_llseek)
				return -EINVAL;
			if (copy_from_user(&offset, off_out, sizeof(loff_t)))
				return -EFAULT;
			off = &offset;
		} else
			off = &out->f_pos;

		ret = do_splice_from(pipe, out, off, len, flags);

		if (off_out && copy_to_user(off_out, off, sizeof(loff_t)))
			ret = -EFAULT;

		return ret;
	}

	pipe = out->f_dentry->d_inode->i_pipe;
	if (pipe) {
		if (off_out)
			return -ESPIPE;
		if (off_in) {
			if (in->f_op->llseek == no_llseek)
				return -EINVAL;
			if (copy_from_user(&offset, off_in, sizeof(loff_t)))
				return -EFAULT;
			off = &offset;
		} else
			off = &in->f_pos;

		ret = do_splice_to(in, off, pipe, len, flags);

		if (off_in && copy_to_user(off_in, off, sizeof(loff_t)))
			ret = -EFAULT;

		return ret;
	}

	return -EINVAL;
}