R_API const char *r_str_last(const char *str, const char *ch) {
	char *ptr, *end = NULL;
	if (!str || !ch) {
		return NULL;
	}
	do {
		ptr = strstr (str, ch);
		if (!ptr) {
			break;
		}
		end = ptr;
		str = ptr + 1;
	} while (true);
	return end;
}