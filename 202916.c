xfs_attrmulti_attr_get(
	struct inode		*inode,
	unsigned char		*name,
	unsigned char		__user *ubuf,
	__uint32_t		*len,
	__uint32_t		flags)
{
	unsigned char		*kbuf;
	int			error = EFAULT;

	if (*len > XATTR_SIZE_MAX)
		return EINVAL;
	kbuf = kmalloc(*len, GFP_KERNEL);
	if (!kbuf)
		return ENOMEM;

	error = xfs_attr_get(XFS_I(inode), name, kbuf, (int *)len, flags);
	if (error)
		goto out_kfree;

	if (copy_to_user(ubuf, kbuf, *len))
		error = EFAULT;

 out_kfree:
	kfree(kbuf);
	return error;
}