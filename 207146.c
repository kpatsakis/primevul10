static int test_mod_exp_consttime(int i)
{
    const MOD_EXP_TEST *test = &ModExpTests[i];
    int res = 0;
    BIGNUM* result = NULL;
    BIGNUM *base = NULL, *exponent = NULL, *modulo = NULL;
    char *s = NULL;

    if (!TEST_ptr(result = BN_new())
            || !TEST_true(BN_dec2bn(&base, test->base))
            || !TEST_true(BN_dec2bn(&exponent, test->exp))
            || !TEST_true(BN_dec2bn(&modulo, test->mod)))
        goto err;

    BN_set_flags(base, BN_FLG_CONSTTIME);
    BN_set_flags(exponent, BN_FLG_CONSTTIME);
    BN_set_flags(modulo, BN_FLG_CONSTTIME);

    if (!TEST_int_eq(BN_mod_exp(result, base, exponent, modulo, ctx), 1))
        goto err;

    if (!TEST_ptr(s = BN_bn2dec(result)))
        goto err;

    if (!TEST_mem_eq(s, strlen(s), test->res, strlen(test->res)))
        goto err;

    res = 1;

 err:
    OPENSSL_free(s);
    BN_free(result);
    BN_free(base);
    BN_free(exponent);
    BN_free(modulo);
    return res;
}