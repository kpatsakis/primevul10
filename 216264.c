R_API int r_str_word_set0(char *str) {
	int i, quote = 0;
	char *p;
	if (!str || !*str) {
		return 0;
	}
	for (i = 0; str[i] && str[i + 1]; i++) {
		if (i > 0 && str[i-1] == ' ' && str[i] == ' ') {
			int len = strlen (str + i);
			memmove (str + i, str + i + 1, len);
			i--;
		}
	}
	if (str[i] == ' ') {
		str[i] = 0;
	}
	for (i = 1, p = str; *p; p++) {
		if (*p == '\"') {
			if (quote) {
				quote = 0;
				*p = '\0';
				// FIX: i++;
				continue;
			} else {
				quote = 1;
				memmove (p, p + 1, strlen (p + 1) + 1);
			}
		}
		if (quote) {
			continue;
		}
		if (*p == ' ') {
			char *q = p - 1;
			if (p > str && (*q == '\\' || !*q)) {
				memmove (p, p + 1, strlen (p + 1) + 1);
				if (*q == '\\') {
					*q = ' ';
					continue;
				}
				p--;
			}
			i++;
			*p = '\0';
		} // s/ /\0/g
	}
	return i;
}