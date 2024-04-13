static char *strchr_skip_color_codes(const char *s, int c) {
	int i = 0;
	for (i = 0; s[i]; i++) {
		while (s[i] && s[i] == 0x1b) {
			while (s[i] && s[i] != 'm') {
				i++;
			}
			if (s[i]) {
				i++;
			}
		}
		if (!s[i] || s[i] == (char)c) {
			return (char*)s + i;
		}
	}
	return NULL;
}