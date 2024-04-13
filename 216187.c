R_API const char *r_sub_str_rchr(const char *str, int start, int end, char chr) {
	while (str[start] != chr && start < end) {
		start++;
	}
	return str[start] == chr ? str + start : NULL;
}