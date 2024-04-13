R_API const char *r_str_word_get0(const char *str, int idx) {
	int i;
	const char *ptr = str;
	if (!ptr || idx < 0 /* prevent crashes with negative index */) {
		return (char *)nullstr;
	}
	for (i = 0; i != idx; i++) {
		ptr += strlen (ptr) + 1;
	}
	return ptr;
}