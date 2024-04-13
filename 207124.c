static int test_kronecker(void)
{
    BIGNUM *a = NULL, *b = NULL, *r = NULL, *t = NULL;
    int i, legendre, kronecker, st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(r = BN_new())
            || !TEST_ptr(t = BN_new()))
        goto err;

    /*
     * We test BN_kronecker(a, b, ctx) just for b odd (Jacobi symbol). In
     * this case we know that if b is prime, then BN_kronecker(a, b, ctx) is
     * congruent to $a^{(b-1)/2}$, modulo $b$ (Legendre symbol). So we
     * generate a random prime b and compare these values for a number of
     * random a's.  (That is, we run the Solovay-Strassen primality test to
     * confirm that b is prime, except that we don't want to test whether b
     * is prime but whether BN_kronecker works.)
     */

    if (!TEST_true(BN_generate_prime_ex(b, 512, 0, NULL, NULL, NULL)))
        goto err;
    BN_set_negative(b, rand_neg());

    for (i = 0; i < NUM0; i++) {
        if (!TEST_true(BN_bntest_rand(a, 512, 0, 0)))
            goto err;
        BN_set_negative(a, rand_neg());

        /* t := (|b|-1)/2  (note that b is odd) */
        if (!TEST_true(BN_copy(t, b)))
            goto err;
        BN_set_negative(t, 0);
        if (!TEST_true(BN_sub_word(t, 1)))
            goto err;
        if (!TEST_true(BN_rshift1(t, t)))
            goto err;
        /* r := a^t mod b */
        BN_set_negative(b, 0);

        if (!TEST_true(BN_mod_exp_recp(r, a, t, b, ctx)))
            goto err;
        BN_set_negative(b, 1);

        if (BN_is_word(r, 1))
            legendre = 1;
        else if (BN_is_zero(r))
            legendre = 0;
        else {
            if (!TEST_true(BN_add_word(r, 1)))
                goto err;
            if (!TEST_int_eq(BN_ucmp(r, b), 0)) {
                TEST_info("Legendre symbol computation failed");
                goto err;
            }
            legendre = -1;
        }

        if (!TEST_int_ge(kronecker = BN_kronecker(a, b, ctx), -1))
            goto err;
        /* we actually need BN_kronecker(a, |b|) */
        if (BN_is_negative(a) && BN_is_negative(b))
            kronecker = -kronecker;

        if (!TEST_int_eq(legendre, kronecker))
            goto err;
    }

    st = 1;
 err:
    BN_free(a);
    BN_free(b);
    BN_free(r);
    BN_free(t);
    return st;
}