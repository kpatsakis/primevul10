R_API size_t r_str_utf8_charsize_prev(const char *str, int prev_len) {
	r_return_val_if_fail (str, 0);
	int pos = 0;
	size_t size = 0, minsize = R_MIN (5, prev_len);
	while (size < minsize) {
		size++;
		if ((str[--pos] & 0xc0) != 0x80) {
			break;
		}
	}
	return size < 5 ? size : 0;
}