R_API void r_str_sanitize(char *c) {
	char *d = c;
	if (d)  {
		for (; *d; c++, d++) {
			switch (*d) {
			case '`':
			case '$':
			case '{':
			case '}':
			case '~':
			case '|':
			case ';':
			case '#':
			case '@':
			case '&':
			case '<':
			case '>':
				*c = '_';
				continue;
			}
		}
	}
}