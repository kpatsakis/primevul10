R_API char *r_str_appendch(char *x, char y) {
	char b[2] = { y, 0 };
	return r_str_append (x, b);
}