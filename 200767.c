asmlinkage long sys_vmsplice(int fd, const struct iovec __user *iov,
			     unsigned long nr_segs, unsigned int flags)
{
	struct file *file;
	long error;
	int fput;

	error = -EBADF;
	file = fget_light(fd, &fput);
	if (file) {
		if (file->f_mode & FMODE_WRITE)
			error = do_vmsplice(file, iov, nr_segs, flags);

		fput_light(file, fput);
	}

	return error;
}