path_mountpoint(struct nameidata *nd, unsigned flags, struct path *path)
{
	const char *s = path_init(nd, flags);
	int err;

	while (!(err = link_path_walk(s, nd)) &&
		(err = lookup_last(nd)) > 0) {
		s = trailing_symlink(nd);
	}
	if (!err && (nd->flags & LOOKUP_RCU))
		err = unlazy_walk(nd);
	if (!err)
		err = handle_lookup_down(nd);
	if (!err) {
		*path = nd->path;
		nd->path.mnt = NULL;
		nd->path.dentry = NULL;
	}
	terminate_walk(nd);
	return err;
}