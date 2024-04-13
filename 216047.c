bool test_r_str_case(void) {
	char* str1_mixedcase = strdup ("mIxEdCaSe");
	char* str2_mixedcase = strdup ("mIxEdCaSe");
	r_str_case (str1_mixedcase, true /*upcase*/);
	r_str_case (str2_mixedcase, false /*downcase*/);
	mu_assert_streq (str1_mixedcase, "MIXEDCASE", "upcase");
	mu_assert_streq (str2_mixedcase, "mixedcase", "downcase");
	char* non_alphanum_1 = strdup ("c00lstring!");
	char* non_alphanum_2 = strdup ("c00lstrinG!");
	r_str_case (non_alphanum_1, true /*upcase*/);
	r_str_case (non_alphanum_2, false /*downcase*/);
	mu_assert_streq (non_alphanum_1, "C00LSTRING!", "upcase, nonalpanum");
	mu_assert_streq (non_alphanum_2, "c00lstring!", "downcase, nonalpanum");
	free (str1_mixedcase);
	free (str2_mixedcase);
	free (non_alphanum_1);
	free (non_alphanum_2);
	mu_end;
}