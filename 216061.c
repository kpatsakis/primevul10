R_API size_t r_str_len_utf8(const char *s) {
	size_t i = 0, j = 0, fullwidths = 0;
	while (s[i]) {
		if ((s[i] & 0xc0) != 0x80) {
			j++;
			if (r_str_char_fullwidth (s + i, 4)) {
				fullwidths++;
			}
		}
		i++;
	}
	return j + fullwidths;
}