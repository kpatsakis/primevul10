bool test_r_str_len_utf8_ansi(void) {
	int len;

	len = r_str_len_utf8_ansi ("radare2");
	mu_assert_eq (len, 7, "len(ascii only)");

	len = r_str_len_utf8_ansi ("r\x1b[38;2;208;80;0madare2");
	mu_assert_eq (len, 7, "len(ascii + ansi ending with m)");

	len = r_str_len_utf8_ansi ("r\x1b[0Jadare2");
	mu_assert_eq (len, 7, "len(ascii + ansi ending with J)");

	len = r_str_len_utf8_ansi ("r\x1b[42;42Hadare2");
	mu_assert_eq (len, 7, "len(ascii + ansi ending with H)");

	len = r_str_len_utf8_ansi ("r\xc3\xa4""dare2");
	mu_assert_eq (len, 7, "len(ascii + 2 byte utf-8 counted as 1 char)");

	len = r_str_len_utf8_ansi ("radar\xe2\x82\xac""2");
	mu_assert_eq (len, 7, "len(ascii + 3 byte utf-8 counted as 1 char)");

	len = r_str_len_utf8_ansi ("radar\xf0\x9d\x84\x9e""2");
	mu_assert_eq (len, 7, "len(ascii + 4 byte utf-8 counted as 1 char)");

	mu_end;
}