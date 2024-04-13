R_API bool r_str_ccmp(const char *dst, const char *src, int ch) {
	int i;
	for (i = 0; src[i] && src[i] != ch; i++) {
		if (dst[i] != src[i]) {
			return true;
		}
	}
	return false;
}