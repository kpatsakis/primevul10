bool test_r_str_newf(void) {
	char *a = r_str_newf ("hello");
	mu_assert_streq (a, "hello", "oops");
	free (a);

	a = r_str_newf ("%s/%s", "hello", "world");
	mu_assert_streq (a, "hello/world", "oops");
	free (a);

	a = r_str_newf ("%s/%s", "hello", "world");
	a = r_str_appendf (a, "..%s/%s", "cow", "low");
	a = r_str_appendf (a, "PWN");
	mu_assert_streq (a, "hello/world..cow/lowPWN", "oops");
	free (a);
	mu_end;
}