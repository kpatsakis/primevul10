bool test_r_str_char_count(void) {
	mu_assert_eq (r_str_char_count ("poop", 'p'), 2, "number of p in poop");
	mu_end;
}