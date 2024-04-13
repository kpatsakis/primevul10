R_API const char *r_str_sep(const char *base, const char *sep) {
	int i;
	while (*base) {
		for (i = 0; sep[i]; i++) {
			if (*base == sep[i]) {
				return base;
			}
		}
		base++;
	}
	return NULL;
}