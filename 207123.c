static int file_modexp(STANZA *s)
{
    BIGNUM *a = NULL, *e = NULL, *m = NULL, *mod_exp = NULL, *ret = NULL;
    BIGNUM *b = NULL, *c = NULL, *d = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(e = getBN(s, "E"))
            || !TEST_ptr(m = getBN(s, "M"))
            || !TEST_ptr(mod_exp = getBN(s, "ModExp"))
            || !TEST_ptr(ret = BN_new())
            || !TEST_ptr(d = BN_new()))
        goto err;

    if (!TEST_true(BN_mod_exp(ret, a, e, m, ctx))
            || !equalBN("A ^ E (mod M)", mod_exp, ret))
        goto err;

    if (BN_is_odd(m)) {
        if (!TEST_true(BN_mod_exp_mont(ret, a, e, m, ctx, NULL))
                || !equalBN("A ^ E (mod M) (mont)", mod_exp, ret)
                || !TEST_true(BN_mod_exp_mont_consttime(ret, a, e, m,
                                                        ctx, NULL))
                || !equalBN("A ^ E (mod M) (mont const", mod_exp, ret))
            goto err;
    }

    /* Regression test for carry propagation bug in sqr8x_reduction */
    BN_hex2bn(&a, "050505050505");
    BN_hex2bn(&b, "02");
    BN_hex2bn(&c,
        "4141414141414141414141274141414141414141414141414141414141414141"
        "4141414141414141414141414141414141414141414141414141414141414141"
        "4141414141414141414141800000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000001");
    if (!TEST_true(BN_mod_exp(d, a, b, c, ctx))
        || !TEST_true(BN_mul(e, a, a, ctx))
        || !TEST_BN_eq(d, e))
        goto err;

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    BN_free(m);
    BN_free(mod_exp);
    BN_free(ret);
    return st;
}