R_API char *r_str_between(const char *cmt, const char *prefix, const char *suffix) {
	char *c0, *c1;
	if (!cmt || !prefix || !suffix || !*cmt) {
		return NULL;
	}
	c0 = strstr (cmt, prefix);
	if (c0) {
		c1 = strstr (c0 + strlen (prefix), suffix);
		if (c1) {
			return r_str_ndup (c0 + strlen (prefix), (c1 - c0 - strlen (prefix)));
		}
	}
	return NULL;
}