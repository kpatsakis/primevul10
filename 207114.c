static int test_gf2m_add(void)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL;
    int i, st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(c = BN_new()))
        goto err;

    for (i = 0; i < NUM0; i++) {
        if (!(TEST_true(BN_rand(a, 512, 0, 0))
                && TEST_ptr(BN_copy(b, BN_value_one()))))
            goto err;
        BN_set_negative(a, rand_neg());
        BN_set_negative(b, rand_neg());
        if (!(TEST_true(BN_GF2m_add(c, a, b))
                /* Test that two added values have the correct parity. */
                && TEST_false((BN_is_odd(a) && BN_is_odd(c))
                        || (!BN_is_odd(a) && !BN_is_odd(c)))))
            goto err;
        if (!(TEST_true(BN_GF2m_add(c, c, c))
                /* Test that c + c = 0. */
                && TEST_BN_eq_zero(c)))
            goto err;
    }
    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    return st;
}