bool test_r_str_format_msvc_argv() {
	// Examples from http://daviddeley.com/autohotkey/parameters/parameters.htm#WINCRULES
	const char *a = "CallMePancake";
	char *str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "CallMePancake", "no escaping");
	free (str);

	a = "Call Me Pancake";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "\"Call Me Pancake\"", "just quoting");
	free (str);

	a = "CallMe\"Pancake";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "CallMe\\\"Pancake", "just escaping");
	free (str);

	a = "CallMePancake\\";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "CallMePancake\\", "no escaping of backslashes");
	free (str);

	a = "Call Me Pancake\\";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "\"Call Me Pancake\\\\\"", "escaping of backslashes before closing quote");
	free (str);

	a = "CallMe\\\"Pancake";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "CallMe\\\\\\\"Pancake", "escaping of backslashes before literal quote");
	free (str);

	a = "Call Me\\\"Pancake";
	str = r_str_format_msvc_argv (1, &a);
	mu_assert_streq (str, "\"Call Me\\\\\\\"Pancake\"", "escaping of backslashes before literal quote in quote");
	free (str);

	const char *args[] = { "rm", "-rf", "\\"};
	str = r_str_format_msvc_argv (3, args);
	mu_assert_streq (str, "rm -rf \\", "multiple args");
	free (str);

	mu_end;
}