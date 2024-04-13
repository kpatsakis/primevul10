R_API size_t r_str_ansi_nlen(const char *str, size_t slen) {
	size_t i = 0, len = 0;
	if (slen > 0) {
		while (str[i] && i < slen) {
			size_t chlen = __str_ansi_length (str + i);
			if (chlen == 1) {
				len ++;
			}
			i += chlen;
		}
		return len > 0 ? len: 1;
	}
	while (str[i]) {
		size_t chlen = __str_ansi_length (str + i);
		if (chlen == 1) {
			len ++;
		}
		i += chlen;
	}
	return len > 0 ? len: 1;
}