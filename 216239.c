R_API int r_str_bounds(const char *_str, int *h) {
	const char *str, *ptr;
	int W = 0, H = 0;
	int cw = 0;

	if (_str) {
		ptr = str = _str;
		while (*str) {
			if (*str == '\n') {
				H++;
				cw = r_str_ansi_nlen (ptr, (size_t)(str - ptr));
				if (cw > W) {
					W = cw;
				}
				cw = 0;
				ptr = str + 1;
			}
			str++;
			cw++;
		}
		if (*str == '\n') {// skip last newline
			H--;
		}
		if (h) {
			*h = H;
		}
	}
	return W;
}