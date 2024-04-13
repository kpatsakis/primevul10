R_API char *r_str_arg_escape(const char *arg) {
	char *str;
	int dest_i = 0, src_i = 0;
	if (!arg) {
		return NULL;
	}
	str = malloc ((2 * strlen (arg) + 1) * sizeof (char)); // Worse case when every character need to be escaped
	if (!str) {
		return NULL;
	}
	for (src_i = 0; arg[src_i] != '\0'; src_i++) {
		char c = arg[src_i];
		switch (c) {
		case '\'':
		case '"':
		case '\\':
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
	return realloc (str, (strlen(str)+1) * sizeof (char));
}