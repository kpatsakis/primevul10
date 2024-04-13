static int dsa_cmp_parameters(const EVP_PKEY *a, const EVP_PKEY *b)
{
    if (BN_cmp(a->pkey.dsa->p, b->pkey.dsa->p) ||
        BN_cmp(a->pkey.dsa->q, b->pkey.dsa->q) ||
        BN_cmp(a->pkey.dsa->g, b->pkey.dsa->g))
        return 0;
    else
        return 1;
}