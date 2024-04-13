bool test_r_str_split(void) {
	char* hi = strdup ("hello world");
	int r = r_str_split (hi, ' ');
	mu_assert_eq (r, 2, "split on space");
	char* hello = hi;
	char* world = hi + 6;
	mu_assert_streq (hello, "hello", "first string in split");
	mu_assert_streq (world, "world", "second string in split");
	free (hi);
	mu_end;
}