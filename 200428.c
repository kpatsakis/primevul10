static __always_inline int link_path_walk(const char *name, struct nameidata *nd)
{
	struct path save = nd->path;
	int result;

	/* make sure the stuff we saved doesn't go away */
	path_get(&save);

	result = __link_path_walk(name, nd);
	if (result == -ESTALE) {
		/* nd->path had been dropped */
		nd->path = save;
		path_get(&nd->path);
		nd->flags |= LOOKUP_REVAL;
		result = __link_path_walk(name, nd);
	}

	path_put(&save);

	return result;
}