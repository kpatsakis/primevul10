static int ibwdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(0, &ibwdt_is_open))
		return -EBUSY;
	if (nowayout)
		__module_get(THIS_MODULE);

	/* Activate */
	ibwdt_ping();
	return nonseekable_open(inode, file);
}