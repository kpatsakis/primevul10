struct dentry *lookup_one_noperm(const char *name, struct dentry *base)
{
	int err;
	struct qstr this;

	err = __lookup_one_len(name, &this, base, strlen(name));
	if (err)
		return ERR_PTR(err);
	return __lookup_hash(&this, base, NULL);
}