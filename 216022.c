R_API void r_str_uri_decode(char *s) {
	int n;
	char *d;
	for (d = s; *s; s++, d++) {
		if (*s == '%') {
			sscanf (s + 1, "%02x", &n);
			*d = n;
			s += 2;
		} else {
			*d = *s;
		}
	}
	*d = 0;
}