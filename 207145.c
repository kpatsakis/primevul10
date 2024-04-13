static int file_sum(STANZA *s)
{
    BIGNUM *a = NULL, *b = NULL, *sum = NULL, *ret = NULL;
    BN_ULONG b_word;
    int st = 0;

    if (!TEST_ptr(a = getBN(s, "A"))
            || !TEST_ptr(b = getBN(s, "B"))
            || !TEST_ptr(sum = getBN(s, "Sum"))
            || !TEST_ptr(ret = BN_new()))
        goto err;

    if (!TEST_true(BN_add(ret, a, b))
            || !equalBN("A + B", sum, ret)
            || !TEST_true(BN_sub(ret, sum, a))
            || !equalBN("Sum - A", b, ret)
            || !TEST_true(BN_sub(ret, sum, b))
            || !equalBN("Sum - B", a, ret))
        goto err;

    /*
     * Test that the functions work when |r| and |a| point to the same BIGNUM,
     * or when |r| and |b| point to the same BIGNUM.
     * There is no test for all of |r|, |a|, and |b| pointint to the same BIGNUM.
     */
    if (!TEST_true(BN_copy(ret, a))
            || !TEST_true(BN_add(ret, ret, b))
            || !equalBN("A + B (r is a)", sum, ret)
            || !TEST_true(BN_copy(ret, b))
            || !TEST_true(BN_add(ret, a, ret))
            || !equalBN("A + B (r is b)", sum, ret)
            || !TEST_true(BN_copy(ret, sum))
            || !TEST_true(BN_sub(ret, ret, a))
            || !equalBN("Sum - A (r is a)", b, ret)
            || !TEST_true(BN_copy(ret, a))
            || !TEST_true(BN_sub(ret, sum, ret))
            || !equalBN("Sum - A (r is b)", b, ret)
            || !TEST_true(BN_copy(ret, sum))
            || !TEST_true(BN_sub(ret, ret, b))
            || !equalBN("Sum - B (r is a)", a, ret)
            || !TEST_true(BN_copy(ret, b))
            || !TEST_true(BN_sub(ret, sum, ret))
            || !equalBN("Sum - B (r is b)", a, ret))
        goto err;

    /*
     * Test BN_uadd() and BN_usub() with the prerequisites they are
     * documented as having. Note that these functions are frequently used
     * when the prerequisites don't hold. In those cases, they are supposed
     * to work as if the prerequisite hold, but we don't test that yet.
     */
    if (!BN_is_negative(a) && !BN_is_negative(b) && BN_cmp(a, b) >= 0) {
        if (!TEST_true(BN_uadd(ret, a, b))
                || !equalBN("A +u B", sum, ret)
                || !TEST_true(BN_usub(ret, sum, a))
                || !equalBN("Sum -u A", b, ret)
                || !TEST_true(BN_usub(ret, sum, b))
                || !equalBN("Sum -u B", a, ret))
            goto err;
        /*
         * Test that the functions work when |r| and |a| point to the same
         * BIGNUM, or when |r| and |b| point to the same BIGNUM.
         * There is no test for all of |r|, |a|, and |b| pointint to the same
         * BIGNUM.
         */
        if (!TEST_true(BN_copy(ret, a))
                || !TEST_true(BN_uadd(ret, ret, b))
                || !equalBN("A +u B (r is a)", sum, ret)
                || !TEST_true(BN_copy(ret, b))
                || !TEST_true(BN_uadd(ret, a, ret))
                || !equalBN("A +u B (r is b)", sum, ret)
                || !TEST_true(BN_copy(ret, sum))
                || !TEST_true(BN_usub(ret, ret, a))
                || !equalBN("Sum -u A (r is a)", b, ret)
                || !TEST_true(BN_copy(ret, a))
                || !TEST_true(BN_usub(ret, sum, ret))
                || !equalBN("Sum -u A (r is b)", b, ret)
                || !TEST_true(BN_copy(ret, sum))
                || !TEST_true(BN_usub(ret, ret, b))
                || !equalBN("Sum -u B (r is a)", a, ret)
                || !TEST_true(BN_copy(ret, b))
                || !TEST_true(BN_usub(ret, sum, ret))
                || !equalBN("Sum -u B (r is b)", a, ret))
            goto err;
    }

    /*
     * Test with BN_add_word() and BN_sub_word() if |b| is small enough.
     */
    b_word = BN_get_word(b);
    if (!BN_is_negative(b) && b_word != (BN_ULONG)-1) {
        if (!TEST_true(BN_copy(ret, a))
                || !TEST_true(BN_add_word(ret, b_word))
                || !equalBN("A + B (word)", sum, ret)
                || !TEST_true(BN_copy(ret, sum))
                || !TEST_true(BN_sub_word(ret, b_word))
                || !equalBN("Sum - B (word)", a, ret))
            goto err;
    }
    st = 1;

 err:
    BN_free(a);
    BN_free(b);
    BN_free(sum);
    BN_free(ret);
    return st;
}