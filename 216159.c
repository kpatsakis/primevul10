R_API char *r_str_newf(const char *fmt, ...) {
	va_list ap, ap2;

	va_start (ap, fmt);
	if (!strchr (fmt, '%')) {
		va_end (ap);
		return strdup (fmt);
	}
	va_copy (ap2, ap);
	int ret = vsnprintf (NULL, 0, fmt, ap2);
	ret++;
	char *p = calloc (1, ret);
	if (p) {
		(void)vsnprintf (p, ret, fmt, ap);
	}
	va_end (ap2);
	va_end (ap);
	return p;
}