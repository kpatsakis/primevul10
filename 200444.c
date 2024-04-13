static int __path_lookup_intent_open(int dfd, const char *name,
		unsigned int lookup_flags, struct nameidata *nd,
		int open_flags, int create_mode)
{
	struct file *filp = get_empty_filp();
	int err;

	if (filp == NULL)
		return -ENFILE;
	nd->intent.open.file = filp;
	nd->intent.open.flags = open_flags;
	nd->intent.open.create_mode = create_mode;
	err = do_path_lookup(dfd, name, lookup_flags|LOOKUP_OPEN, nd);
	if (IS_ERR(nd->intent.open.file)) {
		if (err == 0) {
			err = PTR_ERR(nd->intent.open.file);
			path_put(&nd->path);
		}
	} else if (err != 0)
		release_open_intent(nd);
	return err;
}