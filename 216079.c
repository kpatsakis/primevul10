bool test_r_str_replace(void) {
	// infinite loop test
	char *str = r_str_replace (strdup ("hello world"), "hell", "ihell", 0);
	mu_assert_streq (str, "ihello world", "error, replace char multi failed");
	free (str);

	str = r_str_replace (strdup ("hello world"), "hell", "ihell", 1);
	mu_assert_streq (str, "ihello world", "error, replace char multi failed");
	free (str);

	str = r_str_replace (strdup ("hello world"), "hello", "", 1);
	mu_assert_streq (str, " world", "error, replace char multi failed");
	free (str);

	str = r_str_replace (strdup ("hello world"), "h", "hello", 0);
	mu_assert_streq (str, "helloello world", "error, replace char multi failed");
	free (str);

	str = r_str_replace (strdup ("hello horld"), "h", "hello", 1);
	mu_assert_streq (str, "helloello helloorld", "error, replace char multi failed");
	free (str);
	str = r_str_replace (strdup ("hello horld"), "h", "hello", 0);
	mu_assert_streq (str, "helloello horld", "error, replace char multi failed");
	free (str);
	mu_end;
}