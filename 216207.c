R_API const char *r_str_closer_chr(const char *b, const char *s) {
	const char *a;
	while (*b) {
		for (a = s; *a; a++) {
			if (*b == *a) {
				return b;
			}
		}
		b++;
	}
	return NULL;
}