R_API int r_str_replace_char_once(char *s, int a, int b) {
	return r_str_replace_ch (s, a, b, false);
}