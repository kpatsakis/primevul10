R_API char *r_str_prefix_all(const char *s, const char *pfx) {
	const char *os = s;
	char *p;
	int newlines = 1;
	int len = 0;
	int pfx_len = 0;

	if (!s) {
		return strdup (pfx);
	}
	if (!pfx) {
		return strdup (s);
	}
	len = strlen (s);
	pfx_len = strlen (pfx);
	for (os = s; *os; os++)  {
		if (*os == '\n') {
			newlines++;
		}
	}
	char *o = malloc (len + (pfx_len * newlines) + 1);
	if (!o) {
		return NULL;
	}
	memcpy (o, pfx, pfx_len);
	for (p = o + pfx_len; *s; s++) {
		*p++ = *s;
		if (*s == '\n' && s[1]) {
			memcpy (p, pfx, pfx_len);
			p += pfx_len;
		}
	}
	*p = 0;
	return o;
}