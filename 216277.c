R_API size_t r_str_utf8_charsize(const char *str) {
	r_return_val_if_fail (str, 0);
	size_t size = 0;
	size_t length = strlen (str);
	while (size < length && size < 5) {
		size++;
		if ((str[size] & 0xc0) != 0x80) {
			break;
		}
	}
	return size < 5 ? size : 0;
}