bool test_r_str_utf8_charsize(void) {
	char s[16] = "\x61\xc3\xa1\xe6\x97\xa5\xf0\x9f\x91\x8c\xf0\x9f\x91\x8c\x8c"; // aá日👌
	int sz;

	sz = r_str_utf8_charsize (s);
	mu_assert_eq (sz, 1, "1 byte UTF-8");

	sz = r_str_utf8_charsize (s + 1);
	mu_assert_eq (sz, 2, "2 byte UTF-8");

	sz = r_str_utf8_charsize (s + 3);
	mu_assert_eq (sz, 3, "3 byte UTF-8");

	sz = r_str_utf8_charsize (s + 6);
	mu_assert_eq (sz, 4, "4 byte UTF-8");

	sz = r_str_utf8_charsize (s + 10);
	mu_assert_eq (sz, 0, "Malformed UTF-8");

	mu_end;
}