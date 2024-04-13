static int dsa_copy_parameters(EVP_PKEY *to, const EVP_PKEY *from)
{
    BIGNUM *a;

    if (to->pkey.dsa == NULL) {
        to->pkey.dsa = DSA_new();
        if (to->pkey.dsa == NULL)
            return 0;
    }

    if ((a = BN_dup(from->pkey.dsa->p)) == NULL)
        return 0;
    BN_free(to->pkey.dsa->p);
    to->pkey.dsa->p = a;

    if ((a = BN_dup(from->pkey.dsa->q)) == NULL)
        return 0;
    BN_free(to->pkey.dsa->q);
    to->pkey.dsa->q = a;

    if ((a = BN_dup(from->pkey.dsa->g)) == NULL)
        return 0;
    BN_free(to->pkey.dsa->g);
    to->pkey.dsa->g = a;
    return 1;
}