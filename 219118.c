struct dentry *lookup_one_len_unlocked(const char *name,
				       struct dentry *base, int len)
{
	struct qstr this;
	int err;
	struct dentry *ret;

	err = lookup_one_len_common(name, base, len, &this);
	if (err)
		return ERR_PTR(err);

	ret = lookup_dcache(&this, base, 0);
	if (!ret)
		ret = lookup_slow(&this, base, 0);
	return ret;
}