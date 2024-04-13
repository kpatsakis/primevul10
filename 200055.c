static ssize_t shmem_file_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	read_descriptor_t desc;

	if ((ssize_t) count < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;
	if (!count)
		return 0;

	desc.written = 0;
	desc.count = count;
	desc.arg.buf = buf;
	desc.error = 0;

	do_shmem_file_read(filp, ppos, &desc, file_read_actor);
	if (desc.written)
		return desc.written;
	return desc.error;
}