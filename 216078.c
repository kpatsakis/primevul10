R_API int *r_str_split_lines(char *str, int *count) {
	int i;
	int lines = 0;
	if (!str) {
		return NULL;
	}
	int *indexes = NULL;
	// count lines
	for (i = 0; str[i]; i++) {
		if (str[i] == '\n') {
			lines++;
		}
	}
	// allocate and set indexes
	indexes = calloc (sizeof (int), lines + 1);
	if (!indexes) {
		return NULL;
	}
	int line = 0;
	indexes[line++] = 0;
	for (i = 0; str[i]; i++) {
		if (str[i] == '\n') {
			str[i] = 0;
			indexes[line++] = i + 1;
		}
	}
	if (count) {
		*count = line;
	}
	return indexes;
}