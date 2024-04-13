R_API bool r_str_is_printable_incl_newlines(const char *str) {
	while (*str) {
		int ulen = r_utf8_decode ((const ut8*)str, strlen (str), NULL);
		if (ulen > 1) {
			str += ulen;
			continue;
		}
		if (!IS_PRINTABLE (*str)) {
			if (*str != '\r' && *str != '\n' && *str != '\t') {
				return false;
			}
		}
		str++;
	}
	return true;
}