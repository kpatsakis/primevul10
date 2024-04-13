bool test_r_str_tokenize(void) {
	//XXX r_str_word0 doesn't work on "hello      world" to
	// tokenize into ["hello", "world"]
	char* hi = strdup ("hello world");
	int r = r_str_word_set0 (hi);
	mu_assert_eq (r, 2, "tokenize hello world");
	const char* hello = r_str_word_get0 (hi, 0);
	const char* world = r_str_word_get0 (hi, 1);
	mu_assert_streq (hello, "hello", "first string in split");
	mu_assert_streq (world, "world", "second string in split");
	free (hi);
	mu_end;
}