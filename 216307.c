R_API const char *r_str_rsep(const char *base, const char *p, const char *sep) {
	int i;
	while (p >= base) {
		for (i = 0; sep[i]; i++) {
			if (*p == sep[i]) {
				return p;
			}
		}
		p--;
	}
	return NULL;
}