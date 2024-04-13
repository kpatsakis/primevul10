R_API char *r_str_ichr(char *str, char chr) {
	while (*str == chr) {
		str++;
	}
	return str;
}