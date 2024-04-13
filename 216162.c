bool test_r_str_ichr(void) {
	char* test = "rrrrrradare2";
	char* out = r_str_ichr (test, 'r');
	mu_assert_streq (out, "adare2",
			"string after the first non-r character in rrrrrradare2");
	mu_end;
}