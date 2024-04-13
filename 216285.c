R_API int r_str_split(char *str, char ch) {
	int i;
	char *p;
	if (!str || !*str) {
		return 0;
	}
	/* TODO: sync with r1 code */
	for (i = 1, p = str; *p; p++) {
		if (*p == ch) {
			i++;
			*p = '\0';
		} // s/ /\0/g
	}
	return i;
}