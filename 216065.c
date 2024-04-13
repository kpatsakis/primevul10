R_API char *r_str_trunc_ellipsis(const char *str, int len) {
	if (!str) {
		return NULL;
	}
	if (strlen (str) < len) {
		return strdup (str);
	}
	char *buf = r_str_newlen (str, len);
	if (buf && len > 4) {
		strcpy (buf + len - 4, "...");
	}
	return buf;
}