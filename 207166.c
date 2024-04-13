static int test_ctx_consttime_flag(void)
{
    /*-
     * The constant-time flag should not "leak" among BN_CTX frames:
     *
     * - test_ctx_set_ct_flag() starts a frame in the given BN_CTX and
     *   sets the BN_FLG_CONSTTIME flag on some of the BIGNUMs obtained
     *   from the frame before ending it.
     * - test_ctx_check_ct_flag() then starts a new frame and gets a
     *   number of BIGNUMs from it. In absence of leaks, none of the
     *   BIGNUMs in the new frame should have BN_FLG_CONSTTIME set.
     *
     * In actual BN_CTX usage inside libcrypto the leak could happen at
     * any depth level in the BN_CTX stack, with varying results
     * depending on the patterns of sibling trees of nested function
     * calls sharing the same BN_CTX object, and the effect of
     * unintended BN_FLG_CONSTTIME on the called BN_* functions.
     *
     * This simple unit test abstracts away this complexity and verifies
     * that the leak does not happen between two sibling functions
     * sharing the same BN_CTX object at the same level of nesting.
     *
     */
    BN_CTX *nctx = NULL;
    BN_CTX *sctx = NULL;
    size_t i = 0;
    int st = 0;

    if (!TEST_ptr(nctx = BN_CTX_new())
            || !TEST_ptr(sctx = BN_CTX_secure_new()))
        goto err;

    for (i = 0; i < 2; i++) {
        BN_CTX *c = i == 0 ? nctx : sctx;
        if (!TEST_true(test_ctx_set_ct_flag(c))
                || !TEST_true(test_ctx_check_ct_flag(c)))
            goto err;
    }

    st = 1;
 err:
    BN_CTX_free(nctx);
    BN_CTX_free(sctx);
    return st;
}