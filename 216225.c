R_API const char *r_str_rchr(const char *base, const char *p, int ch) {
	r_return_val_if_fail (base, NULL);
	if (!p) {
		p = base + strlen (base);
	}
	for (; p >= base; p--) {
		if (ch == *p) {
			break;
		}
	}
	return (p >= base) ? p: NULL;
}