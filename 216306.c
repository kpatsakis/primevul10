bool test_r_str_replace_char_once(void) {
	char* str = strdup ("hello world");
	(void) r_str_replace_char_once (str, 'l', 'x');
	mu_assert_streq (str, "hexlo world", "error, replace char once failed");
	free (str);
	mu_end;
}