bool test_r_str_replace_char(void) {
	char* str = strdup ("hello world");
	(void) r_str_replace_char (str, 'l', 'x');
	mu_assert_streq (str, "hexxo worxd", "error, replace char multi failed");
	free (str);
	mu_end;
}