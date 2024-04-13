struct dentry *lookup_one_len(const char *name, struct dentry *base, int len)
{
	int err;
	struct qstr this;

	err = __lookup_one_len(name, &this, base, len);
	if (err)
		return ERR_PTR(err);

	err = permission(base->d_inode, MAY_EXEC, NULL);
	if (err)
		return ERR_PTR(err);
	return __lookup_hash(&this, base, NULL);
}