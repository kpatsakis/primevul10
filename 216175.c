bool test_r_sub_str_rchr(void) {
	const char* test = "raddddare2d";
	const char* out = r_sub_str_rchr (test, 1, 8, 'd');
	mu_assert_streq (out, "ddddare2d", "pointer to first d in range in radddddare2d");
	mu_end;
}