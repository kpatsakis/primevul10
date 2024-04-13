int __user_path_lookup_open(const char __user *name, unsigned int lookup_flags,
		struct nameidata *nd, int open_flags)
{
	char *tmp = getname(name);
	int err = PTR_ERR(tmp);

	if (!IS_ERR(tmp)) {
		err = __path_lookup_intent_open(AT_FDCWD, tmp, lookup_flags, nd, open_flags, 0);
		putname(tmp);
	}
	return err;
}