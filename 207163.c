static int test_signed_mod_replace_ba(int n)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL, *d = NULL;
    int st = 0;

    if (!TEST_ptr(a = set_signed_bn(signed_mod_tests[n].n))
            || !TEST_ptr(b = set_signed_bn(signed_mod_tests[n].divisor))
            || !TEST_ptr(c = set_signed_bn(signed_mod_tests[n].result))
            || !TEST_ptr(d = set_signed_bn(signed_mod_tests[n].remainder)))
        goto err;

    if (TEST_true(BN_div(b, a, a, b, ctx))
            && TEST_BN_eq(b, c)
            && TEST_BN_eq(a, d))
        st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    return st;
}