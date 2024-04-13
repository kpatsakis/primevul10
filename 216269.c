R_API int r_str_replace_ch(char *s, char a, char b, bool global) {
	int ret = 0;
	char *o = s;
	if (!s || a == b) {
		return 0;
	}
	for (; *o; s++, o++) {
		if (*o == a) {
			ret++;
			if (b) {
				*s = b;
			} else {
				/* remove char */
				s--;
			}
			if (!global) {
				return 1;
			}
		} else {
			*s = *o;
		}
	}
	*s = 0;
	return ret;
}