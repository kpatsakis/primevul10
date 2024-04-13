R_API bool r_str_is_printable_limited(const char *str, int size) {
	while (size > 0 && *str) {
		int ulen = r_utf8_decode ((const ut8*)str, strlen (str), NULL);
		if (ulen > 1) {
			str += ulen;
			continue;
		}
		if (!IS_PRINTABLE (*str)) {
			return false;
		}
		str++;
		size--;
	}
	return true;
}