static int test_gf2m_modsolvequad(void)
{
    BIGNUM *a = NULL, *b[2] = {NULL, NULL}, *c = NULL, *d = NULL;
    BIGNUM *e = NULL;
    int i, j, s = 0, t, st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b[0] = BN_new())
            || !TEST_ptr(b[1] = BN_new())
            || !TEST_ptr(c = BN_new())
            || !TEST_ptr(d = BN_new())
            || !TEST_ptr(e = BN_new()))
        goto err;

    if (!(TEST_true(BN_GF2m_arr2poly(p0, b[0]))
            && TEST_true(BN_GF2m_arr2poly(p1, b[1]))))
        goto err;

    for (i = 0; i < NUM0; i++) {
        if (!TEST_true(BN_bntest_rand(a, 512, 0, 0)))
            goto err;
        for (j = 0; j < 2; j++) {
            t = BN_GF2m_mod_solve_quad(c, a, b[j], ctx);
            if (t) {
                s++;
                if (!(TEST_true(BN_GF2m_mod_sqr(d, c, b[j], ctx))
                        && TEST_true(BN_GF2m_add(d, c, d))
                        && TEST_true(BN_GF2m_mod(e, a, b[j]))
                        && TEST_true(BN_GF2m_add(e, e, d))
                        /*
                         * Test that solution of quadratic c
                         * satisfies c^2 + c = a.
                         */
                        && TEST_BN_eq_zero(e)))
                    goto err;
            }
        }
    }
    if (!TEST_int_ge(s, 0)) {
        TEST_info("%d tests found no roots; probably an error", NUM0);
        goto err;
    }
    st = 1;
 err:
    BN_free(a);
    BN_free(b[0]);
    BN_free(b[1]);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    return st;
}