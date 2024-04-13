bool test_r_str_rchr(void) {
	const char* test = "raddddare2d";
	const char* out = r_str_rchr (test, NULL, '2');
	mu_assert_streq (out, "2d", "pointer to last p in range in raddddare2d");
	out = r_str_rchr (test, NULL, 'p');
	if (out) {
		mu_assert ("non NULL value returned", 0);
	}
	out = test + 9;
	out = r_str_rchr (test, out, 'd');
	mu_assert_streq (out, "dare2d", "pointer to last d in range in raddddare2d");
	out = test + strlen (test);
	out = r_str_rchr (test, out, 'p');
	if (out) {
		mu_assert ("non NULL value of out", 0);
	}
	mu_end;
}