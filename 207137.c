static int test_asc2bn(void)
{
    BIGNUM *bn = NULL;
    int st = 0;

    if (!TEST_ptr(bn = BN_new()))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "0"))
            || !TEST_BN_eq_zero(bn)
            || !TEST_BN_ge_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "256"))
            || !TEST_BN_eq_word(bn, 256)
            || !TEST_BN_ge_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "-42"))
            || !TEST_BN_abs_eq_word(bn, 42)
            || !TEST_BN_lt_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "0x1234"))
            || !TEST_BN_eq_word(bn, 0x1234)
            || !TEST_BN_ge_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "0X1234"))
            || !TEST_BN_eq_word(bn, 0x1234)
            || !TEST_BN_ge_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "-0xabcd"))
            || !TEST_BN_abs_eq_word(bn, 0xabcd)
            || !TEST_BN_lt_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "-0"))
            || !TEST_BN_eq_zero(bn)
            || !TEST_BN_ge_zero(bn))
        goto err;

    if (!TEST_true(BN_asc2bn(&bn, "123trailing garbage is ignored"))
            || !TEST_BN_eq_word(bn, 123)
            || !TEST_BN_ge_zero(bn))
        goto err;

    st = 1;
 err:
    BN_free(bn);
    return st;
}