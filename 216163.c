R_API void r_str_filter_zeroline(char *str, int len) {
	int i;
	for (i = 0; i < len && str[i]; i++) {
		if (str[i] == '\n' || str[i] == '\r') {
			break;
		}
		if (!IS_PRINTABLE (str[i])) {
			break;
		}
	}
	str[i] = 0;
}