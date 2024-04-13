static int test_rand(void)
{
    BIGNUM *bn = NULL;
    int st = 0;

    if (!TEST_ptr(bn = BN_new()))
        return 0;

    /* Test BN_rand for degenerate cases with |top| and |bottom| parameters. */
    if (!TEST_false(BN_rand(bn, 0, 0 /* top */ , 0 /* bottom */ ))
            || !TEST_false(BN_rand(bn, 0, 1 /* top */ , 1 /* bottom */ ))
            || !TEST_true(BN_rand(bn, 1, 0 /* top */ , 0 /* bottom */ ))
            || !TEST_BN_eq_one(bn)
            || !TEST_false(BN_rand(bn, 1, 1 /* top */ , 0 /* bottom */ ))
            || !TEST_true(BN_rand(bn, 1, -1 /* top */ , 1 /* bottom */ ))
            || !TEST_BN_eq_one(bn)
            || !TEST_true(BN_rand(bn, 2, 1 /* top */ , 0 /* bottom */ ))
            || !TEST_BN_eq_word(bn, 3))
        goto err;

    st = 1;
 err:
    BN_free(bn);
    return st;
}