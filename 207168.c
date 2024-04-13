static int test_signed_mod_replace_ab(int n)
{
    BIGNUM *a = NULL, *b = NULL, *c = NULL, *d = NULL;
    int st = 0;

    if (!TEST_ptr(a = set_signed_bn(signed_mod_tests[n].n))
            || !TEST_ptr(b = set_signed_bn(signed_mod_tests[n].divisor))
            || !TEST_ptr(c = set_signed_bn(signed_mod_tests[n].result))
            || !TEST_ptr(d = set_signed_bn(signed_mod_tests[n].remainder)))
        goto err;

    if (TEST_true(BN_div(a, b, a, b, ctx))
            && TEST_BN_eq(a, c)
            && TEST_BN_eq(b, d))
        st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    return st;
}