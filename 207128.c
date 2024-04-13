static int test_ctx_check_ct_flag(BN_CTX *c)
{
    int st = 0;
    size_t i;
    BIGNUM *b[30];

    BN_CTX_start(c);
    for (i = 0; i < OSSL_NELEM(b); i++) {
        if (!TEST_ptr(b[i] = BN_CTX_get(c)))
            goto err;
        if (!TEST_false(BN_get_flags(b[i], BN_FLG_CONSTTIME)))
            goto err;
    }

    st = 1;
 err:
    BN_CTX_end(c);
    return st;
}