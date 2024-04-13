R_API const char *r_sub_str_lchr(const char *str, int start, int end, char chr) {
	do {
		end--;
	} while (str[end] != chr && end >= start);
	return str[end] == chr ? &str[end] : NULL;
}