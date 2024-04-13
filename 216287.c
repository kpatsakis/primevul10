R_API size_t r_str_utf8_charsize_last(const char *str) {
	r_return_val_if_fail (str, 0);
	size_t len = strlen (str);
	return r_str_utf8_charsize_prev (str + len, len);
}