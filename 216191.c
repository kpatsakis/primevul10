R_API void r_str_remove_char(char *str, char c) {
	while (*str) {
		if (*str == c) {
			memmove (str, str + 1, strlen (str + 1) + 1);
			continue;
		}
		str++;
	}
}