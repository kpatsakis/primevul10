R_API const char *r_str_nstr(const char *s, const char *find, int slen) {
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen (find);
		do {
			do {
				if (slen-- < 1 || !(sc = *s++)) {
					return NULL;
				}
			} while (sc != c);
			if (len > slen) {
				return NULL;
			}
		} while (strncmp (s, find, len) != 0);
		s--;
	}
	return (char *)s;
}