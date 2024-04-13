bool test_r_str_rwx_i(void) {
	const char* rwx = r_str_rwx_i (7);
	const char* rw = r_str_rwx_i (6);
	const char* rx = r_str_rwx_i (5);
	const char* invalid_mode = r_str_rwx_i (898);
	const char* invalid_mode_neg = r_str_rwx_i (-10);
	mu_assert_streq (rwx, "rwx", "rwx = 7 mode");
	mu_assert_streq (rw, "rw-", "rw = 6 mode");
	mu_assert_streq (rx, "r-x", "rx = 5 mode");
	mu_assert_streq (invalid_mode, "---", "invalid permissions mode");
	mu_assert_streq (invalid_mode_neg, "---", "invalid permissions mode (negative value)");
	mu_end;
}