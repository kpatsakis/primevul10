R_API void r_str_truncate_cmd(char *string) {
	ut32 pos = 0;
	if (string && *string) {
		ut32 sz = strlen (string);
		for (pos = 0; pos < sz; pos++) {
			switch (string[pos]) {
			case '!':
			case ':':
			case ';':
			case '@':
			case '~':
			case '(':
			case '[':
			case '{':
			case '?':
				string[pos] = '\0';
				return;
			}
		}
	}
}