static int test_sub(void)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL;
    int i, st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(c = BN_new()))
        goto err;

    for (i = 0; i < NUM0 + NUM1; i++) {
        if (i < NUM1) {
            if (!(TEST_true(BN_bntest_rand(a, 512, 0, 0)))
                    && TEST_ptr(BN_copy(b, a))
                    && TEST_int_ne(BN_set_bit(a, i), 0)
                    && TEST_true(BN_add_word(b, i)))
                goto err;
        } else {
            if (!TEST_true(BN_bntest_rand(b, 400 + i - NUM1, 0, 0)))
                goto err;
            BN_set_negative(a, rand_neg());
            BN_set_negative(b, rand_neg());
        }
        if (!(TEST_true(BN_sub(c, a, b))
                && TEST_true(BN_add(c, c, b))
                && TEST_true(BN_sub(c, c, a))
                && TEST_BN_eq_zero(c)))
            goto err;
    }
    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    return st;
}