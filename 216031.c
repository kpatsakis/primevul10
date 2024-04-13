R_API void r_str_reverse(char *str) {
	int i, len = strlen (str);
	int half = len / 2;
	for (i = 0; i < half; i++) {
		char ch = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = ch;
	}
}