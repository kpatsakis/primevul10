static int test_smallsafeprime(int kBits)
{
    BIGNUM *r;
    int st = 0;

    if (!TEST_ptr(r = BN_new()))
        goto err;

    if (kBits <= 5 && kBits != 3) {
        if (!TEST_false(BN_generate_prime_ex(r, kBits, 1,
                                             NULL, NULL, NULL)))
            goto err;
    } else {
        if (!TEST_true(BN_generate_prime_ex(r, kBits, 1,
                                            NULL, NULL, NULL))
                || !TEST_int_eq(BN_num_bits(r), kBits))
            goto err;
    }

    st = 1;
 err:
    BN_free(r);
    return st;
}