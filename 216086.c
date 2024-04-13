R_API int r_str_char_count(const char *string, char ch) {
	int i, count = 0;
	for (i = 0; string[i]; i++) {
		if (string[i] == ch) {
			count++;
		}
	}
	return count;
}