R_API const char *r_strstr_ansi(const char *a, const char *b) {
	const char *ch, *p = a;
	do {
		ch = strchr (p, '\x1b');
		if (ch) {
			const char *v = r_str_nstr (p, b, ch - p);
			if (v) {
				return v;
			}
			p = ch + __str_ansi_length (ch);
		}
	} while (ch);
	return strstr (p, b);
}