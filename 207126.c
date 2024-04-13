static int test_hex2bn(void)
{
    BIGNUM *bn = NULL;
    int st = 0;

    if (!TEST_int_eq(parseBN(&bn, "0"), 1)
            || !TEST_BN_eq_zero(bn)
            || !TEST_BN_ge_zero(bn)
            || !TEST_BN_even(bn))
        goto err;
    BN_free(bn);
    bn = NULL;

    if (!TEST_int_eq(parseBN(&bn, "256"), 3)
            || !TEST_BN_eq_word(bn, 0x256)
            || !TEST_BN_ge_zero(bn)
            || !TEST_BN_gt_zero(bn)
            || !TEST_BN_ne_zero(bn)
            || !TEST_BN_even(bn))
        goto err;
    BN_free(bn);
    bn = NULL;

    if (!TEST_int_eq(parseBN(&bn, "-42"), 3)
            || !TEST_BN_abs_eq_word(bn, 0x42)
            || !TEST_BN_lt_zero(bn)
            || !TEST_BN_le_zero(bn)
            || !TEST_BN_ne_zero(bn)
            || !TEST_BN_even(bn))
        goto err;
    BN_free(bn);
    bn = NULL;

    if (!TEST_int_eq(parseBN(&bn, "cb"), 2)
            || !TEST_BN_eq_word(bn, 0xCB)
            || !TEST_BN_ge_zero(bn)
            || !TEST_BN_gt_zero(bn)
            || !TEST_BN_ne_zero(bn)
            || !TEST_BN_odd(bn))
        goto err;
    BN_free(bn);
    bn = NULL;

    if (!TEST_int_eq(parseBN(&bn, "-0"), 2)
            || !TEST_BN_eq_zero(bn)
            || !TEST_BN_ge_zero(bn)
            || !TEST_BN_le_zero(bn)
            || !TEST_BN_even(bn))
        goto err;
    BN_free(bn);
    bn = NULL;

    if (!TEST_int_eq(parseBN(&bn, "abctrailing garbage is ignored"), 3)
            || !TEST_BN_eq_word(bn, 0xabc)
            || !TEST_BN_ge_zero(bn)
            || !TEST_BN_gt_zero(bn)
            || !TEST_BN_ne_zero(bn)
            || !TEST_BN_even(bn))
        goto err;
    st = 1;

 err:
    BN_free(bn);
    return st;
}