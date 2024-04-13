int setup_tests(void)
{
    OPTION_CHOICE o;
    int n, stochastic = 0;

    while ((o = opt_next()) != OPT_EOF) {
        switch (o) {
        case OPT_STOCHASTIC_TESTS:
            stochastic = 1;
            break;
        case OPT_TEST_CASES:
           break;
        default:
        case OPT_ERR:
            return 0;
        }
    }
    n  = test_get_argument_count();

    if (!TEST_ptr(ctx = BN_CTX_new()))
        return 0;

    if (n == 0) {
        ADD_TEST(test_sub);
        ADD_TEST(test_div_recip);
        ADD_ALL_TESTS(test_signed_mod_replace_ab, OSSL_NELEM(signed_mod_tests));
        ADD_ALL_TESTS(test_signed_mod_replace_ba, OSSL_NELEM(signed_mod_tests));
        ADD_TEST(test_mod);
        ADD_TEST(test_modexp_mont5);
        ADD_TEST(test_kronecker);
        ADD_TEST(test_rand);
        ADD_TEST(test_bn2padded);
        ADD_TEST(test_dec2bn);
        ADD_TEST(test_hex2bn);
        ADD_TEST(test_asc2bn);
        ADD_ALL_TESTS(test_mpi, (int)OSSL_NELEM(kMPITests));
        ADD_TEST(test_negzero);
        ADD_TEST(test_badmod);
        ADD_TEST(test_expmodzero);
        ADD_TEST(test_expmodone);
        ADD_ALL_TESTS(test_smallprime, 16);
        ADD_ALL_TESTS(test_smallsafeprime, 16);
        ADD_TEST(test_swap);
        ADD_TEST(test_ctx_consttime_flag);
#ifndef OPENSSL_NO_EC2M
        ADD_TEST(test_gf2m_add);
        ADD_TEST(test_gf2m_mod);
        ADD_TEST(test_gf2m_mul);
        ADD_TEST(test_gf2m_sqr);
        ADD_TEST(test_gf2m_modinv);
        ADD_TEST(test_gf2m_moddiv);
        ADD_TEST(test_gf2m_modexp);
        ADD_TEST(test_gf2m_modsqrt);
        ADD_TEST(test_gf2m_modsolvequad);
#endif
        ADD_ALL_TESTS(test_is_prime, (int)OSSL_NELEM(primes));
        ADD_ALL_TESTS(test_not_prime, (int)OSSL_NELEM(not_primes));
        ADD_TEST(test_gcd_prime);
        ADD_ALL_TESTS(test_mod_exp, (int)OSSL_NELEM(ModExpTests));
        ADD_ALL_TESTS(test_mod_exp_consttime, (int)OSSL_NELEM(ModExpTests));
        if (stochastic)
            ADD_TEST(test_rand_range);
    } else {
        ADD_ALL_TESTS(run_file_tests, n);
    }
    return 1;
}