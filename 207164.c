static int test_rand_range_single(size_t n)
{
    const unsigned int range = rand_range_cases[n].range;
    const unsigned int iterations = rand_range_cases[n].iterations;
    const double critical = rand_range_cases[n].critical;
    const double expected = iterations / (double)range;
    double sum = 0;
    BIGNUM *rng = NULL, *val = NULL;
    size_t *counts;
    unsigned int i, v;
    int res = 0;

    if (!TEST_ptr(counts = OPENSSL_zalloc(sizeof(*counts) * range))
        || !TEST_ptr(rng = BN_new())
        || !TEST_ptr(val = BN_new())
        || !TEST_true(BN_set_word(rng, range)))
        goto err;
    for (i = 0; i < iterations; i++) {
        if (!TEST_true(BN_rand_range(val, rng))
            || !TEST_uint_lt(v = (unsigned int)BN_get_word(val), range))
            goto err;
        counts[v]++;
    }

    for (i = 0; i < range; i++) {
        const double delta = counts[i] - expected;
        sum += delta * delta;
    }
    sum /= expected;

    if (sum > critical) {
        TEST_info("Chi^2 test negative %.4f > %4.f", sum, critical);
        TEST_note("test case %zu  range %u  iterations %u", n + 1, range,
                  iterations);
        goto err;
    }

    res = 1;
err:
    BN_free(rng);
    BN_free(val);
    OPENSSL_free(counts);
    return res;
}