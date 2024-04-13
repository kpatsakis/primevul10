static int shmem_xattr_security_get(struct inode *inode, const char *name,
				    void *buffer, size_t size)
{
	if (strcmp(name, "") == 0)
		return -EINVAL;
	return security_inode_getsecurity(inode, name, buffer, size,
					  -EOPNOTSUPP);
}