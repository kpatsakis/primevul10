bool test_r_str_word_count(void) {
	mu_assert_eq (r_str_word_count ("let's test\nradare2 \t libraries!"), 4,
				"words in a string");
	mu_end;
}