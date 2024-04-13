bool test_r_sub_str_lchr(void) {
	const char* test = "raddddare2d";
	const char* out = r_sub_str_lchr (test, 1, 8, 'd');
	mu_assert_streq (out, "dare2d", "pointer to last d in range in radddddare2d");
	mu_end;
}