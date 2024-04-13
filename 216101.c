R_API char *r_str_dup(char *ptr, const char *string) {
	free (ptr);
	return r_str_new (string);
}