R_API char *r_str_path_escape(const char *path) {
	char *str;
	int dest_i = 0, src_i = 0;

	if (!path) {
		return NULL;
	}
	// Worst case when every character need to be escaped
	str = malloc ((2 * strlen (path) + 1) * sizeof (char));
	if (!str) {
		return NULL;
	}

	for (src_i = 0; path[src_i] != '\0'; src_i++) {
		char c = path[src_i];
		switch (c) {
		case ' ':
			str[dest_i++] = '\\';
			str[dest_i++] = c;
			break;
		default:
			str[dest_i++] = c;
			break;
		}
	}

	str[dest_i] = '\0';
	return realloc (str, (strlen (str) + 1) * sizeof (char));
}