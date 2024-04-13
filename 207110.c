static int test_is_prime(int i)
{
    int ret = 0;
    BIGNUM *r = NULL;
    int trial;

    if (!TEST_ptr(r = BN_new()))
        goto err;

    for (trial = 0; trial <= 1; ++trial) {
        if (!TEST_true(BN_set_word(r, primes[i]))
                || !TEST_int_eq(BN_check_prime(r, ctx, NULL),
                                1))
            goto err;
    }

    ret = 1;
 err:
    BN_free(r);
    return ret;
}