static int test_modexp_mont5(void)
{
    BIGNUM *a = NULL, *p = NULL, *m = NULL, *d = NULL, *e = NULL;
    BIGNUM *b = NULL, *n = NULL, *c = NULL;
    BN_MONT_CTX *mont = NULL;
    int st = 0;

    if (!TEST_ptr(a = BN_new())
            || !TEST_ptr(p = BN_new())
            || !TEST_ptr(m = BN_new())
            || !TEST_ptr(d = BN_new())
            || !TEST_ptr(e = BN_new())
            || !TEST_ptr(b = BN_new())
            || !TEST_ptr(n = BN_new())
            || !TEST_ptr(c = BN_new())
            || !TEST_ptr(mont = BN_MONT_CTX_new()))
        goto err;

    /* must be odd for montgomery */
    if (!(TEST_true(BN_bntest_rand(m, 1024, 0, 1))
            /* Zero exponent */
            && TEST_true(BN_bntest_rand(a, 1024, 0, 0))))
        goto err;
    BN_zero(p);

    if (!TEST_true(BN_mod_exp_mont_consttime(d, a, p, m, ctx, NULL)))
        goto err;
    if (!TEST_BN_eq_one(d))
        goto err;

    /* Regression test for carry bug in mulx4x_mont */
    if (!(TEST_true(BN_hex2bn(&a,
        "7878787878787878787878787878787878787878787878787878787878787878"
        "7878787878787878787878787878787878787878787878787878787878787878"
        "7878787878787878787878787878787878787878787878787878787878787878"
        "7878787878787878787878787878787878787878787878787878787878787878"))
        && TEST_true(BN_hex2bn(&b,
        "095D72C08C097BA488C5E439C655A192EAFB6380073D8C2664668EDDB4060744"
        "E16E57FB4EDB9AE10A0CEFCDC28A894F689A128379DB279D48A2E20849D68593"
        "9B7803BCF46CEBF5C533FB0DD35B080593DE5472E3FE5DB951B8BFF9B4CB8F03"
        "9CC638A5EE8CDD703719F8000E6A9F63BEED5F2FCD52FF293EA05A251BB4AB81"))
        && TEST_true(BN_hex2bn(&n,
        "D78AF684E71DB0C39CFF4E64FB9DB567132CB9C50CC98009FEB820B26F2DED9B"
        "91B9B5E2B83AE0AE4EB4E0523CA726BFBE969B89FD754F674CE99118C3F2D1C5"
        "D81FDC7C54E02B60262B241D53C040E99E45826ECA37A804668E690E1AFC1CA4"
        "2C9A15D84D4954425F0B7642FC0BD9D7B24E2618D2DCC9B729D944BADACFDDAF"))))
        goto err;

    if (!(TEST_true(BN_MONT_CTX_set(mont, n, ctx))
            && TEST_true(BN_mod_mul_montgomery(c, a, b, mont, ctx))
            && TEST_true(BN_mod_mul_montgomery(d, b, a, mont, ctx))
            && TEST_BN_eq(c, d)))
        goto err;

    /* Regression test for carry bug in sqr[x]8x_mont */
    if (!(TEST_true(parse_bigBN(&n, bn1strings))
            && TEST_true(parse_bigBN(&a, bn2strings))))
        goto err;
    BN_free(b);
    if (!(TEST_ptr(b = BN_dup(a))
            && TEST_true(BN_MONT_CTX_set(mont, n, ctx))
            && TEST_true(BN_mod_mul_montgomery(c, a, a, mont, ctx))
            && TEST_true(BN_mod_mul_montgomery(d, a, b, mont, ctx))
            && TEST_BN_eq(c, d)))
        goto err;

    /* Regression test for carry bug in bn_sqrx8x_internal */
    {
        static const char *ahex[] = {
                      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8FFEADBCFC4DAE7FFF908E92820306B",
            "9544D954000000006C0000000000000000000000000000000000000000000000",
            "00000000000000000000FF030202FFFFF8FFEBDBCFC4DAE7FFF908E92820306B",
            "9544D954000000006C000000FF0302030000000000FFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF01FC00FF02FFFFFFFF",
            "00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FCFD",
            "FCFFFFFFFFFF000000000000000000FF0302030000000000FFFFFFFFFFFFFFFF",
            "FF00FCFDFDFF030202FF00000000FFFFFFFFFFFFFFFFFF00FCFDFCFFFFFFFFFF",
            NULL
        };
        static const char *nhex[] = {
                      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8F8F8F8000000",
            "00000010000000006C0000000000000000000000000000000000000000000000",
            "00000000000000000000000000000000000000FFFFFFFFFFFFF8F8F8F8000000",
            "00000010000000006C000000000000000000000000FFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFF000000000000000000000000000000000000FFFFFFFFFFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
            NULL
        };

        if (!(TEST_true(parse_bigBN(&a, ahex))
                && TEST_true(parse_bigBN(&n, nhex))))
            goto err;
    }
    BN_free(b);
    if (!(TEST_ptr(b = BN_dup(a))
            && TEST_true(BN_MONT_CTX_set(mont, n, ctx))))
        goto err;

    if (!TEST_true(BN_mod_mul_montgomery(c, a, a, mont, ctx))
            || !TEST_true(BN_mod_mul_montgomery(d, a, b, mont, ctx))
            || !TEST_BN_eq(c, d))
        goto err;

    /* Regression test for bug in BN_from_montgomery_word */
    if (!(TEST_true(BN_hex2bn(&a,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"))
         && TEST_true(BN_hex2bn(&n,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"))
        && TEST_true(BN_MONT_CTX_set(mont, n, ctx))
        && TEST_false(BN_mod_mul_montgomery(d, a, a, mont, ctx))))
        goto err;

    /* Regression test for bug in rsaz_1024_mul_avx2 */
    if (!(TEST_true(BN_hex2bn(&a,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2020202020DF"))
        && TEST_true(BN_hex2bn(&b,
        "2020202020202020202020202020202020202020202020202020202020202020"
        "2020202020202020202020202020202020202020202020202020202020202020"
        "20202020202020FF202020202020202020202020202020202020202020202020"
        "2020202020202020202020202020202020202020202020202020202020202020"))
        && TEST_true(BN_hex2bn(&n,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2020202020FF"))
        && TEST_true(BN_MONT_CTX_set(mont, n, ctx))
        && TEST_true(BN_mod_exp_mont_consttime(c, a, b, n, ctx, mont))
        && TEST_true(BN_mod_exp_mont(d, a, b, n, ctx, mont))
        && TEST_BN_eq(c, d)))
        goto err;

    /*
     * rsaz_1024_mul_avx2 expects fully-reduced inputs.
     * BN_mod_exp_mont_consttime should reduce the input first.
     */
    if (!(TEST_true(BN_hex2bn(&a,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2020202020DF"))
        && TEST_true(BN_hex2bn(&b,
        "1FA53F26F8811C58BE0357897AA5E165693230BC9DF5F01DFA6A2D59229EC69D"
        "9DE6A89C36E3B6957B22D6FAAD5A3C73AE587B710DBE92E83D3A9A3339A085CB"
        "B58F508CA4F837924BB52CC1698B7FDC2FD74362456A595A5B58E38E38E38E38"
        "E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E38E"))
        && TEST_true(BN_hex2bn(&n,
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2020202020DF"))
        && TEST_true(BN_MONT_CTX_set(mont, n, ctx))
        && TEST_true(BN_mod_exp_mont_consttime(c, a, b, n, ctx, mont))))
        goto err;
    BN_zero(d);
    if (!TEST_BN_eq(c, d))
        goto err;

    /*
     * Regression test for overflow bug in bn_sqr_comba4/8 for
     * mips-linux-gnu and mipsel-linux-gnu 32bit targets.
     */
    {
        static const char *ehex[] = {
            "95564994a96c45954227b845a1e99cb939d5a1da99ee91acc962396ae999a9ee",
            "38603790448f2f7694c242a875f0cad0aae658eba085f312d2febbbd128dd2b5",
            "8f7d1149f03724215d704344d0d62c587ae3c5939cba4b9b5f3dc5e8e911ef9a",
            "5ce1a5a749a4989d0d8368f6e1f8cdf3a362a6c97fb02047ff152b480a4ad985",
            "2d45efdf0770542992afca6a0590d52930434bba96017afbc9f99e112950a8b1",
            "a359473ec376f329bdae6a19f503be6d4be7393c4e43468831234e27e3838680",
            "b949390d2e416a3f9759e5349ab4c253f6f29f819a6fe4cbfd27ada34903300e",
            "da021f62839f5878a36f1bc3085375b00fd5fa3e68d316c0fdace87a97558465",
            NULL};
        static const char *phex[] = {
            "f95dc0f980fbd22e90caa5a387cc4a369f3f830d50dd321c40db8c09a7e1a241",
            "a536e096622d3280c0c1ba849c1f4a79bf490f60006d081e8cf69960189f0d31",
            "2cd9e17073a3fba7881b21474a13b334116cb2f5dbf3189a6de3515d0840f053",
            "c776d3982d391b6d04d642dda5cc6d1640174c09875addb70595658f89efb439",
            "dc6fbd55f903aadd307982d3f659207f265e1ec6271b274521b7a5e28e8fd7a5",
            "5df089292820477802a43cf5b6b94e999e8c9944ddebb0d0e95a60f88cb7e813",
            "ba110d20e1024774107dd02949031864923b3cb8c3f7250d6d1287b0a40db6a4",
            "7bd5a469518eb65aa207ddc47d8c6e5fc8e0c105be8fc1d4b57b2e27540471d5",
            NULL};
        static const char *mhex[] = {
            "fef15d5ce4625f1bccfbba49fc8439c72bf8202af039a2259678941b60bb4a8f",
            "2987e965d58fd8cf86a856674d519763d0e1211cc9f8596971050d56d9b35db3",
            "785866cfbca17cfdbed6060be3629d894f924a89fdc1efc624f80d41a22f1900",
            "9503fcc3824ef62ccb9208430c26f2d8ceb2c63488ec4c07437aa4c96c43dd8b",
            "9289ed00a712ff66ee195dc71f5e4ead02172b63c543d69baf495f5fd63ba7bc",
            "c633bd309c016e37736da92129d0b053d4ab28d21ad7d8b6fab2a8bbdc8ee647",
            "d2fbcf2cf426cf892e6f5639e0252993965dfb73ccd277407014ea784aaa280c",
            "b7b03972bc8b0baa72360bdb44b82415b86b2f260f877791cd33ba8f2d65229b",
            NULL};

        if (!TEST_true(parse_bigBN(&e, ehex))
                || !TEST_true(parse_bigBN(&p, phex))
                || !TEST_true(parse_bigBN(&m, mhex))
                || !TEST_true(BN_mod_exp_mont_consttime(d, e, p, m, ctx, NULL))
                || !TEST_true(BN_mod_exp_simple(a, e, p, m, ctx))
                || !TEST_BN_eq(a, d))
            goto err;
    }

    /* Zero input */
    if (!TEST_true(BN_bntest_rand(p, 1024, 0, 0)))
        goto err;
    BN_zero(a);
    if (!TEST_true(BN_mod_exp_mont_consttime(d, a, p, m, ctx, NULL))
            || !TEST_BN_eq_zero(d))
        goto err;

    /*
     * Craft an input whose Montgomery representation is 1, i.e., shorter
     * than the modulus m, in order to test the const time precomputation
     * scattering/gathering.
     */
    if (!(TEST_true(BN_one(a))
            && TEST_true(BN_MONT_CTX_set(mont, m, ctx))))
        goto err;
    if (!TEST_true(BN_from_montgomery(e, a, mont, ctx))
            || !TEST_true(BN_mod_exp_mont_consttime(d, e, p, m, ctx, NULL))
            || !TEST_true(BN_mod_exp_simple(a, e, p, m, ctx))
            || !TEST_BN_eq(a, d))
        goto err;

    /* Finally, some regular test vectors. */
    if (!(TEST_true(BN_bntest_rand(e, 1024, 0, 0))
            && TEST_true(BN_mod_exp_mont_consttime(d, e, p, m, ctx, NULL))
            && TEST_true(BN_mod_exp_simple(a, e, p, m, ctx))
            && TEST_BN_eq(a, d)))
        goto err;

    st = 1;

 err:
    BN_MONT_CTX_free(mont);
    BN_free(a);
    BN_free(p);
    BN_free(m);
    BN_free(d);
    BN_free(e);
    BN_free(b);
    BN_free(n);
    BN_free(c);
    return st;
}