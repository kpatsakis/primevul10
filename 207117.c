static int test_gcd_prime(void)
{
    BIGNUM *a = NULL, *b = NULL, *gcd = NULL;
    int i, st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(gcd = BN_new()))
        goto err;

    if (!TEST_true(BN_generate_prime_ex(a, 1024, 0, NULL, NULL, NULL)))
            goto err;
    for (i = 0; i < NUM0; i++) {
        if (!TEST_true(BN_generate_prime_ex(b, 1024, 0,
                                            NULL, NULL, NULL))
                || !TEST_true(BN_gcd(gcd, a, b, ctx))
                || !TEST_true(BN_is_one(gcd)))
            goto err;
    }

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(gcd);
    return st;
}