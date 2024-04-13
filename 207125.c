static int test_not_prime(int i)
{
    int ret = 0;
    BIGNUM *r = NULL;
    int trial;

    if (!TEST_ptr(r = BN_new()))
        goto err;

    for (trial = 0; trial <= 1; ++trial) {
        if (!TEST_true(BN_set_word(r, not_primes[i]))
                || !TEST_false(BN_check_prime(r, ctx, NULL)))
            goto err;
    }

    ret = 1;
 err:
    BN_free(r);
    return ret;
}