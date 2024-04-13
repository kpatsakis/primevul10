bool test_r_str_constpool(void) {
	RStrConstPool pool;
	bool s = r_str_constpool_init (&pool);
	mu_assert ("pool init success", s);

	const char *a_ref = "deliverance";
	const char *a_pooled = r_str_constpool_get (&pool, a_ref);
	mu_assert_ptrneq (a_pooled, a_ref, "pooled != ref");
	mu_assert_streq (a_pooled, a_ref, "pooled == ref (strcmp)");
	const char *a_pooled2 = r_str_constpool_get (&pool, a_ref);
	mu_assert_ptreq (a_pooled2, a_pooled, "same on re-get");
	char *a_ref_cpy = strdup (a_ref);
	a_pooled2 = r_str_constpool_get (&pool, a_ref_cpy);
	free (a_ref_cpy);
	mu_assert_ptreq (a_pooled2, a_pooled, "same on re-get with different ptr");

	const char *b_ref = "damnation";
	const char *b_pooled = r_str_constpool_get (&pool, b_ref);
	mu_assert_ptrneq (b_pooled, b_ref, "pooled != ref (second)");
	mu_assert_streq (b_pooled, b_ref, "pooled == ref (strcmp, second)");

	r_str_constpool_fini (&pool);
	mu_end;
}