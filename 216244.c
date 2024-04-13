bool test_r_str_bool(void) {
	const char* one = r_str_bool(1);
	const char* zero = r_str_bool(0);
	const char* fifty = r_str_bool(50);
	const char* negative = r_str_bool(-1);
	mu_assert_streq (one, "true", "one");
	mu_assert_streq (zero, "false", "zero");
	mu_assert_streq (fifty, "true", "large positive value");
	mu_assert_streq (negative, "true", "negative number");
	mu_end;
}