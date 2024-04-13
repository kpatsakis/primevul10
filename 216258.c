R_API void r_str_ncpy(char *dst, const char *src, size_t n) {
	int i;

	// do not do anything if n is 0
	if (n == 0) {
		return;
	}

	n--;
	for (i = 0; src[i] && n > 0; i++, n--) {
		dst[i] = src[i];
	}
	dst[i] = 0;
}