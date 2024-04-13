bool test_r_str_escape_sh(void) {
	char *escaped = r_str_escape_sh ("Hello, \"World\"");
	mu_assert_streq (escaped, "Hello, \\\"World\\\"", "escaped \"double quotes\"");
	free (escaped);
	escaped = r_str_escape_sh ("Hello, \\World\\");
	mu_assert_streq (escaped, "Hello, \\\\World\\\\", "escaped backspace");
	free (escaped);
#if __UNIX__
	escaped = r_str_escape_sh ("Hello, $(World)");
	mu_assert_streq (escaped, "Hello, \\$(World)", "escaped $(command)");
	free (escaped);
	escaped = r_str_escape_sh ("Hello, `World`");
	mu_assert_streq (escaped, "Hello, \\`World\\`", "escaped `command`");
	free (escaped);
#endif
	mu_end;
}