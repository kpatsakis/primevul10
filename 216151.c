bool test_r_str_unescape(void) {
	char buf[] = "Hello\\x31World\\n";
	r_str_unescape (buf);
	mu_assert_streq (buf, "Hello1World\n", "unescaped");
	mu_end;
}