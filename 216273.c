bool test_r_str_str_xy(void) {
	char *canvas = "Hello World\n"
		"This World is World\n"
		"World is Hello\n";
	int x = 0, y = 0;
	const char *next = r_str_str_xy (canvas, "World", NULL, &x, &y);
	mu_assert_eq (x, 6, "x of first occurrence");
	mu_assert_eq (y, 0, "y of first occurrence");
	next = r_str_str_xy (canvas, "World", next, &x, &y);
	mu_assert_eq (x, 5, "x of second occurrence");
	mu_assert_eq (y, 1, "y of second occurrence");
	next = r_str_str_xy (canvas, "World", next, &x, &y);
	mu_assert_eq (x, 14, "x of third occurrence");
	mu_assert_eq (y, 1, "y of third occurrence");
	next = r_str_str_xy (canvas, "World", next, &x, &y);
	mu_assert_eq (x, 0, "x of fourth occurrence");
	mu_assert_eq (y, 2, "y of fourth occurrence");
	next = r_str_str_xy (canvas, "World", next, &x, &y);
	mu_assert_null (next, "no more occurences");
	mu_end;
}