static int test_div_recip(void)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL, *d = NULL, *e = NULL;
    BN_RECP_CTX *recp = NULL;
    int st = 0, i;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(c = BN_new())
            || !TEST_ptr(d = BN_new())
            || !TEST_ptr(e = BN_new())
            || !TEST_ptr(recp = BN_RECP_CTX_new()))
        goto err;

    for (i = 0; i < NUM0 + NUM1; i++) {
        if (i < NUM1) {
            if (!(TEST_true(BN_bntest_rand(a, 400, 0, 0))
                    && TEST_ptr(BN_copy(b, a))
                    && TEST_true(BN_lshift(a, a, i))
                    && TEST_true(BN_add_word(a, i))))
                goto err;
        } else {
            if (!(TEST_true(BN_bntest_rand(b, 50 + 3 * (i - NUM1), 0, 0))))
                goto err;
        }
        BN_set_negative(a, rand_neg());
        BN_set_negative(b, rand_neg());
        if (!(TEST_true(BN_RECP_CTX_set(recp, b, ctx))
                && TEST_true(BN_div_recp(d, c, a, recp, ctx))
                && TEST_true(BN_mul(e, d, b, ctx))
                && TEST_true(BN_add(d, e, c))
                && TEST_true(BN_sub(d, d, a))
                && TEST_BN_eq_zero(d)))
            goto err;
    }
    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    BN_RECP_CTX_free(recp);
    return st;
}