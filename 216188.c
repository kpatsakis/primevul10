R_API void r_str_filter(char *str, int len) {
	size_t i;
	if (len < 1) {
		len = strlen (str);
	}
	for (i = 0; i < len; i++) {
		if (!IS_PRINTABLE (str[i])) {
			str[i] = '.';
		}
	}
}