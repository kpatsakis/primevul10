R_API int r_str_path_unescape(char *path) {
	int i;

	for (i = 0; path[i]; i++) {
		if (path[i] != '\\') {
			continue;
		}
		if (path[i + 1] == ' ') {
			path[i] = ' ';
			memmove (path + i + 1, path + i + 2, strlen (path + i + 2) + 1);
		}
	}

	return i;
}