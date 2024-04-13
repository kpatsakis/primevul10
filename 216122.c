R_API size_t r_wstr_clen(const char *s) {
	size_t len = 0;
	if (!*s++) {
		return 0;
	}
	while (*s++ || *s++) {
		len++;
	}
	return len + 1;
}