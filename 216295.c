bool test_r_str_bits64(void) {
	char buf[65];
	(void)r_str_bits64 (buf, 0);
	mu_assert_streq (buf, "00000000", "binary of 0");
	(void)r_str_bits64 (buf, 1);
	mu_assert_streq (buf, "00000001", "binary of 1");
	(void)r_str_bits64 (buf, 2);
	mu_assert_streq (buf, "00000010", "binary of 2");
	mu_end;
}