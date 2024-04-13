R_API bool r_str_is_ascii(const char *str) {
	const ut8 *ptr;
	for (ptr = (const ut8 *)str; *ptr; ptr++) {
		if (*ptr > 0x7f) {
			return false;
		}
	}
	return true;
}