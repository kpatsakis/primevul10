bool test_r_str_trim(void) {
	//  1
	const char* one = r_str_trim_head_ro ("  hello  ");
	mu_assert_streq (one, "hello  ", "one");
	//  2
	char* two = strdup ("  hello  ");
	r_str_trim_head (two);
	mu_assert_streq (two, "hello  ", "two");
	r_str_trim (two);
	//  2
	mu_assert_streq (two, "hello", "three");
	free (two);
	mu_end;
}