struct dentry *lookup_positive_unlocked(const char *name,
				       struct dentry *base, int len)
{
	struct dentry *ret = lookup_one_len_unlocked(name, base, len);
	if (!IS_ERR(ret) && d_flags_negative(smp_load_acquire(&ret->d_flags))) {
		dput(ret);
		ret = ERR_PTR(-ENOENT);
	}
	return ret;
}