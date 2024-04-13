R_API char *r_str_sanitize_sdb_key(const char *s) {
	if (!s || !*s) {
		return NULL;
	}
	size_t len = strlen (s);
	char *ret = malloc (len + 1);
	if (!ret) {
		return NULL;
	}
	char *cur = ret;
	while (len > 0) {
		char c = *s;
		if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9')
			&& c != '_' && c != ':') {
			c = '_';
		}
		*cur = c;
		s++;
		cur++;
		len--;
	}
	*cur = '\0';
	return ret;
}