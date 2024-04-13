static int path_lookup_create(int dfd, const char *name,
			      unsigned int lookup_flags, struct nameidata *nd,
			      int open_flags, int create_mode)
{
	return __path_lookup_intent_open(dfd, name, lookup_flags|LOOKUP_CREATE,
			nd, open_flags, create_mode);
}