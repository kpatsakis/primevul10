static ssize_t aio_setup_vectored_rw(int type, struct kiocb *kiocb, bool compat)
{
	ssize_t ret;

#ifdef CONFIG_COMPAT
	if (compat)
		ret = compat_rw_copy_check_uvector(type,
				(struct compat_iovec __user *)kiocb->ki_buf,
				kiocb->ki_nbytes, 1, &kiocb->ki_inline_vec,
				&kiocb->ki_iovec);
	else
#endif
		ret = rw_copy_check_uvector(type,
				(struct iovec __user *)kiocb->ki_buf,
				kiocb->ki_nbytes, 1, &kiocb->ki_inline_vec,
				&kiocb->ki_iovec);
	if (ret < 0)
		goto out;

	kiocb->ki_nr_segs = kiocb->ki_nbytes;
	kiocb->ki_cur_seg = 0;
	/* ki_nbytes/left now reflect bytes instead of segs */
	kiocb->ki_nbytes = ret;
	kiocb->ki_left = ret;

	ret = 0;
out:
	return ret;
}