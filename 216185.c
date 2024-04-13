R_API char *r_str_utf16_encode(const char *s, int len) {
	int i;
	char ch[4], *d, *od, *tmp;
	if (!s) {
		return NULL;
	}
	if (len < 0) {
		len = strlen (s);
	}
	if ((len * 7) + 1 < len) {
		return NULL;
	}
	od = d = malloc (1 + (len * 7));
	if (!d) {
		return NULL;
	}
	for (i = 0; i < len; s++, i++) {
		if (*s == '\\') {
			*d++ = '\\';
			*d++ = '\\';
		} else if (*s == '"') {
			*d++ = '\\';
			*d++ = '"';
		} else if ((*s >= 0x20) && (*s <= 126)) {
			*d++ = *s;
		} else {
			*d++ = '\\';
			//	*d++ = '\\';
			*d++ = 'u';
			*d++ = '0';
			*d++ = '0';
			snprintf (ch, sizeof (ch), "%02x", 0xff & ((ut8)*s));
			*d++ = ch[0];
			*d++ = ch[1];
		}
	}
	*d = 0;
	tmp = realloc (od, strlen (od) + 1); // FIT
	if (!tmp) {
		free (od);
		return NULL;
	}
	return tmp;
}