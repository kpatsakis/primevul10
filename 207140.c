static int test_badmod(void)
{
    BIGNUM *a = NULL, *b = NULL, *zero = NULL;
    BN_MONT_CTX *mont = NULL;
    int st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(zero = BN_new())
            || !TEST_ptr(mont = BN_MONT_CTX_new()))
        goto err;
    BN_zero(zero);

    if (!TEST_false(BN_div(a, b, BN_value_one(), zero, ctx)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_mul(a, BN_value_one(), BN_value_one(), zero, ctx)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_exp(a, BN_value_one(), BN_value_one(), zero, ctx)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_exp_mont(a, BN_value_one(), BN_value_one(),
                                    zero, ctx, NULL)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_exp_mont_consttime(a, BN_value_one(), BN_value_one(),
                                              zero, ctx, NULL)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_MONT_CTX_set(mont, zero, ctx)))
        goto err;
    ERR_clear_error();

    /* Some operations also may not be used with an even modulus. */
    if (!TEST_true(BN_set_word(b, 16)))
        goto err;

    if (!TEST_false(BN_MONT_CTX_set(mont, b, ctx)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_exp_mont(a, BN_value_one(), BN_value_one(),
                                    b, ctx, NULL)))
        goto err;
    ERR_clear_error();

    if (!TEST_false(BN_mod_exp_mont_consttime(a, BN_value_one(), BN_value_one(),
                                              b, ctx, NULL)))
        goto err;
    ERR_clear_error();

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(zero);
    BN_MONT_CTX_free(mont);
    return st;
}