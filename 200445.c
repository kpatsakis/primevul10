int __user_walk_fd(int dfd, const char __user *name, unsigned flags,
			    struct nameidata *nd)
{
	char *tmp = getname(name);
	int err = PTR_ERR(tmp);

	if (!IS_ERR(tmp)) {
		err = do_path_lookup(dfd, tmp, flags, nd);
		putname(tmp);
	}
	return err;
}