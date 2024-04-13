R_API char *r_str_appendlen(char *ptr, const char *string, int slen) {
	char *msg = r_str_newlen (string, slen);
	char *ret = r_str_append (ptr, msg);
	free (msg);
	return ret;
}