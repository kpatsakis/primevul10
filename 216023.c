R_API char *r_str_new(const char *str) {
	return str? strdup (str): NULL;
}