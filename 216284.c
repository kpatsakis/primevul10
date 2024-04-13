bool test_r_str_rwx(void) {
	int rwx = r_str_rwx ("rwx");
	int rw =  r_str_rwx ("rw-");
	int rx = r_str_rwx ("rx");
	int none = r_str_rwx ("---");
	int number = r_str_rwx ("999");
	int rx_number = r_str_rwx ("5");
	int rwx_number = r_str_rwx ("7");
	mu_assert_eq (rwx, 7, "rwx");
	mu_assert_eq (rw, 6, "rw");
	mu_assert_eq (rx, 5, "rx");
	mu_assert_eq (none, 0, "no permissions");
	mu_assert_eq (number, 0, "large input number string");
	mu_assert_eq (rx_number, 5, "rx number");
	mu_assert_eq (rwx_number, 7, "rwx number");
	mu_end;
}