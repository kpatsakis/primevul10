static int file_modsqrt(STANZA *s)
{
    BIGNUM *a = NULL, *p = NULL, *mod_sqrt = NULL, *ret = NULL, *ret2 = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(p = getBN(s, "P"))
            || !TEST_ptr(mod_sqrt = getBN(s, "ModSqrt"))
            || !TEST_ptr(ret = BN_new())
            || !TEST_ptr(ret2 = BN_new()))
        goto err;

    /* There are two possible answers. */
    if (!TEST_true(BN_mod_sqrt(ret, a, p, ctx))
            || !TEST_true(BN_sub(ret2, p, ret)))
        goto err;

    /* The first condition should NOT be a test. */
    if (BN_cmp(ret2, mod_sqrt) != 0
            && !equalBN("sqrt(A) (mod P)", mod_sqrt, ret))
        goto err;

    st = 1;
 err:
    BN_free(a);
    BN_free(p);
    BN_free(mod_sqrt);
    BN_free(ret);
    BN_free(ret2);
    return st;
}