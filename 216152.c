R_API const char *r_str_ansi_chrn(const char *str, size_t n) {
	int len, i, li;
	for (li = i = len = 0; str[i] && (n != len); i++) {
		size_t chlen = __str_ansi_length (str + i);
		if (chlen > 1) {
			i += chlen - 1;
		} else {
			if ((str[i] & 0xc0) != 0x80) {
				len++;
			}
			li = i;
		}
	}
	return str + li;
}