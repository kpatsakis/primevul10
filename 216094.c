R_API const char *r_sys_prefix(const char *pfx) {
	static char *prefix = NULL;
	if (!prefix) {
#if __WINDOWS__
		prefix = r_sys_get_src_dir_w32 ();
		if (!prefix) {
			prefix = strdup (R2_PREFIX);
		}
#else
		prefix = strdup (R2_PREFIX);
#endif
	}
	if (pfx) {
		free (prefix);
		prefix = strdup (pfx);
	}
	return prefix;
}