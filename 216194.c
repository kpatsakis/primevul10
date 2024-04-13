R_API const char * r_str_tok(const char *str1, const char b, size_t len) {
	const char *p = str1;
	size_t i = 0;
	if (!p || !*p) {
		return p;
	}
	if (len == -1) {
		len = strlen (str1);
	}
	for ( ; i < len; i++,p++) {
		if (*p == b) {
			break;
		}
	}
	if (i == len) {
		p = NULL;
	}
	return p;
}