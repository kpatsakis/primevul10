xfs_attrmulti_attr_set(
	struct inode		*inode,
	unsigned char		*name,
	const unsigned char	__user *ubuf,
	__uint32_t		len,
	__uint32_t		flags)
{
	unsigned char		*kbuf;
	int			error = EFAULT;

	if (IS_IMMUTABLE(inode) || IS_APPEND(inode))
		return EPERM;
	if (len > XATTR_SIZE_MAX)
		return EINVAL;

	kbuf = memdup_user(ubuf, len);
	if (IS_ERR(kbuf))
		return PTR_ERR(kbuf);

	error = xfs_attr_set(XFS_I(inode), name, kbuf, len, flags);

	return error;
}