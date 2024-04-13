R_API char *r_str_append_owned(char *ptr, char *string) {
	if (!ptr) {
		return string;
	}
	char *r = r_str_append(ptr, string);
	free (string);
	return r;
}