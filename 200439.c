int path_lookup_open(int dfd, const char *name, unsigned int lookup_flags,
		struct nameidata *nd, int open_flags)
{
	return __path_lookup_intent_open(dfd, name, lookup_flags, nd,
			open_flags, 0);
}