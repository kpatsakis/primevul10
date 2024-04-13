R_API char *r_str_uri_encode(const char *s) {
	char ch[4], *d, *od;
	if (!s) {
		return NULL;
	}
	od = d = malloc (1 + (strlen (s) * 4));
	if (!d) {
		return NULL;
	}
	for (; *s; s++) {
		if((*s>='0' && *s<='9')
		|| (*s>='a' && *s<='z')
		|| (*s>='A' && *s<='Z')) {
			*d++ = *s;
		} else {
			*d++ = '%';
			snprintf (ch, sizeof (ch), "%02x", 0xff & ((ut8)*s));
			*d++ = ch[0];
			*d++ = ch[1];
		}
	}
	*d = 0;
	char *trimDown = realloc (od, strlen (od) + 1); // FIT
	return trimDown? trimDown: od;
}