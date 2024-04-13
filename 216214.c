R_API char *r_str_escape_sh(const char *buf) {
	r_return_val_if_fail (buf, NULL);
	char *new_buf = malloc (1 + strlen (buf) * 2);
	if (!new_buf) {
		return NULL;
	}
	const char *p = buf;
	char *q = new_buf;
	while (*p) {
		switch (*p) {
#if __UNIX__
		case '$':
		case '`':
#endif
		case '\\':
		case '"':
			*q++ = '\\';
			/* FALLTHRU */
		default:
			*q++ = *p++;
			break;
		}
	}
	*q = '\0';
	return new_buf;
}