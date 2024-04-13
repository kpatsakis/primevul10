R_API int r_str_ccpy(char *dst, char *src, int ch) {
	int i;
	for (i = 0; src[i] && src[i] != ch; i++) {
		dst[i] = src[i];
	}
	dst[i] = '\0';
	return i;
}