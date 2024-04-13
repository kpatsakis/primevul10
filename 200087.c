static size_t shmem_xattr_security_list(struct inode *inode, char *list,
					size_t list_len, const char *name,
					size_t name_len)
{
	return security_inode_listsecurity(inode, list, list_len);
}