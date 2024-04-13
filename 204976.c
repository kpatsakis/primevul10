static int dsa_missing_parameters(const EVP_PKEY *pkey)
{
    DSA *dsa;
    dsa = pkey->pkey.dsa;
    if ((dsa->p == NULL) || (dsa->q == NULL) || (dsa->g == NULL))
        return 1;
    return 0;
}