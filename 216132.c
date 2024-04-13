R_API size_t r_str_len_utf8char(const char *s, int left) {
	size_t i = 1;
	while (s[i] && (!left || i<left)) {
		if ((s[i] & 0xc0) != 0x80) {
			i++;
		} else {
			break;
		}
	}
	return i;
}