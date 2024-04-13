static int file_square(STANZA *s)
{
    BIGNUM *a = NULL, *square = NULL, *zero = NULL, *ret = NULL;
    BIGNUM *remainder = NULL, *tmp = NULL;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(square = getBN(s, "Square"))
            || !TEST_ptr(zero = BN_new())
            || !TEST_ptr(ret = BN_new())
            || !TEST_ptr(remainder = BN_new()))
        goto err;

    BN_zero(zero);
    if (!TEST_true(BN_sqr(ret, a, ctx))
            || !equalBN("A^2", square, ret)
            || !TEST_true(BN_mul(ret, a, a, ctx))
            || !equalBN("A * A", square, ret)
            || !TEST_true(BN_div(ret, remainder, square, a, ctx))
            || !equalBN("Square / A", a, ret)
            || !equalBN("Square % A", zero, remainder))
        goto err;

#if HAVE_BN_SQRT
    BN_set_negative(a, 0);
    if (!TEST_true(BN_sqrt(ret, square, ctx))
            || !equalBN("sqrt(Square)", a, ret))
        goto err;

    /* BN_sqrt should fail on non-squares and negative numbers. */
    if (!TEST_BN_eq_zero(square)) {
        if (!TEST_ptr(tmp = BN_new())
                || !TEST_true(BN_copy(tmp, square)))
            goto err;
        BN_set_negative(tmp, 1);

        if (!TEST_int_eq(BN_sqrt(ret, tmp, ctx), 0))
            goto err;
        ERR_clear_error();

        BN_set_negative(tmp, 0);
        if (BN_add(tmp, tmp, BN_value_one()))
            goto err;
        if (!TEST_int_eq(BN_sqrt(ret, tmp, ctx)))
            goto err;
        ERR_clear_error();
    }
#endif

    st = 1;
 err:
    BN_free(a);
    BN_free(square);
    BN_free(zero);
    BN_free(ret);
    BN_free(remainder);
    BN_free(tmp);
    return st;
}