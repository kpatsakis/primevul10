static int test_mod(void)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL, *d = NULL, *e = NULL;
    int st = 0, i;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(c = BN_new())
            || !TEST_ptr(d = BN_new())
            || !TEST_ptr(e = BN_new()))
        goto err;

    if (!(TEST_true(BN_bntest_rand(a, 1024, 0, 0))))
        goto err;
    for (i = 0; i < NUM0; i++) {
        if (!(TEST_true(BN_bntest_rand(b, 450 + i * 10, 0, 0))))
            goto err;
        BN_set_negative(a, rand_neg());
        BN_set_negative(b, rand_neg());
        if (!(TEST_true(BN_mod(c, a, b, ctx))
                && TEST_true(BN_div(d, e, a, b, ctx))
                && TEST_BN_eq(e, c)
                && TEST_true(BN_mul(c, d, b, ctx))
                && TEST_true(BN_add(d, c, e))
                && TEST_BN_eq(d, a)))
            goto err;
    }
    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    return st;
}