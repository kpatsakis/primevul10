R_API int r_str_cmp(const char *a, const char *b, int len) {
	if ((a == b) || (!a && !b)) {
		return 0;
	}
	if (!a && b) {
		return -1;
	}
	if (a && !b) {
		return 1;
	}
	if (len < 0) {
		return strcmp (a, b);
	}
	return strncmp (a, b, len);
}