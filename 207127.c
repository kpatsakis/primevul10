static int test_expmodzero(void)
{
    BIGNUM *a = NULL, *r = NULL, *zero = NULL;
    int st = 0;

    if (!TEST_ptr(zero = BN_new())
            || !TEST_ptr(a = BN_new())
            || !TEST_ptr(r = BN_new()))
        goto err;
    BN_zero(zero);

    if (!TEST_true(BN_mod_exp(r, a, zero, BN_value_one(), NULL))
            || !TEST_BN_eq_zero(r)
            || !TEST_true(BN_mod_exp_mont(r, a, zero, BN_value_one(),
                                          NULL, NULL))
            || !TEST_BN_eq_zero(r)
            || !TEST_true(BN_mod_exp_mont_consttime(r, a, zero,
                                                    BN_value_one(),
                                                    NULL, NULL))
            || !TEST_BN_eq_zero(r)
            || !TEST_true(BN_mod_exp_mont_word(r, 42, zero,
                                               BN_value_one(), NULL, NULL))
            || !TEST_BN_eq_zero(r))
        goto err;

    st = 1;
 err:
    BN_free(zero);
    BN_free(a);
    BN_free(r);
    return st;
}