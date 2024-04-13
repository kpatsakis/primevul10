static int file_exp(STANZA *s)
{
    BIGNUM *a = NULL, *e = NULL, *exp = NULL, *ret = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(e = getBN(s, "E"))
            || !TEST_ptr(exp = getBN(s, "Exp"))
            || !TEST_ptr(ret = BN_new()))
        goto err;

    if (!TEST_true(BN_exp(ret, a, e, ctx))
            || !equalBN("A ^ E", exp, ret))
        goto err;

    st = 1;
 err:
    BN_free(a);
    BN_free(e);
    BN_free(exp);
    BN_free(ret);
    return st;
}