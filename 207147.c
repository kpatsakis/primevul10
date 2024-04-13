static int file_gcd(STANZA *s)
{
    BIGNUM *a = NULL, *b = NULL, *gcd = NULL, *ret = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(b = getBN(s, "B"))
            || !TEST_ptr(gcd = getBN(s, "GCD"))
            || !TEST_ptr(ret = BN_new()))
        goto err;

    if (!TEST_true(BN_gcd(ret, a, b, ctx))
            || !equalBN("gcd(A,B)", gcd, ret))
        goto err;

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(gcd);
    BN_free(ret);
    return st;
}