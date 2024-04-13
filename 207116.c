static int test_rand_range(void)
{
    int n_success = 0;
    size_t i;

    for (i = 0; i < OSSL_NELEM(rand_range_cases); i++)
        n_success += test_rand_range_single(i);
    if (TEST_int_ge(n_success, binomial_critical))
        return 1;
    TEST_note("This test is expected to fail by chance 0.01%% of the time.");
    return 0;
}