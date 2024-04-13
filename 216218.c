R_API char *r_str_word_get_first(const char *text) {
	for (; *text && IS_SEPARATOR (*text); text++) {
		;
	}
	return strdup (text);
}