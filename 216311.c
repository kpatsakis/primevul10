bool test_r_str_sanitize_sdb_key(void) {
	char *s = r_str_sanitize_sdb_key("rada.re2<is>::Cool");
	mu_assert_streq (s, "rada_re2_is_::Cool", "sanitize");
	free (s);
	mu_end;
}