bool test_r_str_lchr(void) {
	const char* test = "radare2";
	const char* out = r_str_lchr (test, 'r');
	mu_assert_streq (out, "re2", "pointer to last r in radare2");
	mu_end;
}