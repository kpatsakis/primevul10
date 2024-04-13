R_API char *r_str_newlen(const char *str, int len) {
	if (len < 1) {
		return NULL;
	}
	char *buf = malloc (len + 1);
	if (buf) {
		memcpy (buf, str, len);
		buf[len] = 0;
	}
	return buf;
}