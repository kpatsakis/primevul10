int SRP_create_verifier_BN(const char *user, const char *pass, BIGNUM **salt,
                           BIGNUM **verifier, const BIGNUM *N,
                           const BIGNUM *g)
{
    int result = 0;
    BIGNUM *x = NULL;
    BN_CTX *bn_ctx = BN_CTX_new();
    unsigned char tmp2[MAX_LEN];
    BIGNUM *salttmp = NULL;

    if ((user == NULL) ||
        (pass == NULL) ||
        (salt == NULL) ||
        (verifier == NULL) || (N == NULL) || (g == NULL) || (bn_ctx == NULL))
        goto err;

    if (*salt == NULL) {
        if (RAND_bytes(tmp2, SRP_RANDOM_SALT_LEN) <= 0)
            goto err;

        salttmp = BN_bin2bn(tmp2, SRP_RANDOM_SALT_LEN, NULL);
    } else {
        salttmp = *salt;
    }

    x = SRP_Calc_x(salttmp, user, pass);

    *verifier = BN_new();
    if (*verifier == NULL)
        goto err;

    if (!BN_mod_exp(*verifier, g, x, N, bn_ctx)) {
        BN_clear_free(*verifier);
        goto err;
    }

    result = 1;
    *salt = salttmp;

 err:
    if (salt != NULL && *salt != salttmp)
        BN_clear_free(salttmp);
    BN_clear_free(x);
    BN_CTX_free(bn_ctx);
    return result;
}