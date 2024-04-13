R_API const char *r_str_rstr(const char *base, const char *p) {
	char *s = strdup (base);
	char *k = strdup (p);
	r_str_reverse (s);
	r_str_reverse (k);
	char *q = strstr (s, k);
	const char *r = NULL;
	if (q) {
		r = base + strlen (base) - (q - s) - strlen (p);
	}
	free (s);
	free (k);
	return r;
}