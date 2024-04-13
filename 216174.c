R_API int r_str_fmtargs(const char *fmt) {
	int n = 0;
	while (*fmt) {
		if (*fmt == '%') {
			if (fmt[1] == '*') {
				n++;
			}
			n++;
		}
		fmt++;
	}
	return n;
}