R_API const char *r_str_lchr(const char *str, char chr) {
	if (str) {
		int len = strlen (str);
		for (; len >= 0; len--) {
			if (str[len] == chr) {
				return str + len;
			}
		}
	}
	return NULL;
}