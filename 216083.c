R_API size_t r_str_len_utf8_ansi(const char *str) {
	int i = 0, len = 0, fullwidths = 0;
	while (str[i]) {
		char ch = str[i];
		size_t chlen = __str_ansi_length (str + i);
		if (chlen > 1) {
			i += chlen - 1;
		} else if ((ch & 0xc0) != 0x80) { // utf8
			len++;
			if (r_str_char_fullwidth (str + i, 4)) {
				fullwidths++;
			}
		}
		i++;
	}
	return len + fullwidths;
}