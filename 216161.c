R_API int r_str_word_count(const char *string) {
	const char *text, *tmp;
	int word;

	for (text = tmp = string; *text && IS_SEPARATOR (*text); text++) {
		;
	}
	for (word = 0; *text; word++) {
		for (; *text && !IS_SEPARATOR (*text); text++) {
			;
		}
		for (tmp = text; *text && IS_SEPARATOR (*text); text++) {
			;
		}
	}
	return word;
}