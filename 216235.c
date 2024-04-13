R_API char *r_str_nextword(char *s, char ch) {
	char *p = strchr (s, ch);
	if (!p) {
		return NULL;
	}
	*p++ = 0;
	return p;
}