static int dsa_param_decode(EVP_PKEY *pkey,
                            const unsigned char **pder, int derlen)
{
    DSA *dsa;

    if ((dsa = d2i_DSAparams(NULL, pder, derlen)) == NULL) {
        DSAerr(DSA_F_DSA_PARAM_DECODE, ERR_R_DSA_LIB);
        return 0;
    }
    EVP_PKEY_assign_DSA(pkey, dsa);
    return 1;
}