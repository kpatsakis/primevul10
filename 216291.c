R_API size_t r_str_nlen(const char *str, int n) {
	size_t len = 0;
	if (str) {
		while (*str && n > 0) {
			len++;
			str++;
			n--;
		}
	}
	return len;
}