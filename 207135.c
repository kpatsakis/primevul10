static int file_modmul(STANZA *s)
{
    BIGNUM *a = NULL, *b = NULL, *m = NULL, *mod_mul = NULL, *ret = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(b = getBN(s, "B"))
            || !TEST_ptr(m = getBN(s, "M"))
            || !TEST_ptr(mod_mul = getBN(s, "ModMul"))
            || !TEST_ptr(ret = BN_new()))
        goto err;

    if (!TEST_true(BN_mod_mul(ret, a, b, m, ctx))
            || !equalBN("A * B (mod M)", mod_mul, ret))
        goto err;

    if (BN_is_odd(m)) {
        /* Reduce |a| and |b| and test the Montgomery version. */
        BN_MONT_CTX *mont = BN_MONT_CTX_new();
        BIGNUM *a_tmp = BN_new();
        BIGNUM *b_tmp = BN_new();

        if (mont == NULL || a_tmp == NULL || b_tmp == NULL
                || !TEST_true(BN_MONT_CTX_set(mont, m, ctx))
                || !TEST_true(BN_nnmod(a_tmp, a, m, ctx))
                || !TEST_true(BN_nnmod(b_tmp, b, m, ctx))
                || !TEST_true(BN_to_montgomery(a_tmp, a_tmp, mont, ctx))
                || !TEST_true(BN_to_montgomery(b_tmp, b_tmp, mont, ctx))
                || !TEST_true(BN_mod_mul_montgomery(ret, a_tmp, b_tmp,
                                                    mont, ctx))
                || !TEST_true(BN_from_montgomery(ret, ret, mont, ctx))
                || !equalBN("A * B (mod M) (mont)", mod_mul, ret))
            st = 0;
        else
            st = 1;
        BN_MONT_CTX_free(mont);
        BN_free(a_tmp);
        BN_free(b_tmp);
        if (st == 0)
            goto err;
    }

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(m);
    BN_free(mod_mul);
    BN_free(ret);
    return st;
}