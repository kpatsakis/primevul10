static int old_dsa_priv_decode(EVP_PKEY *pkey,
                               const unsigned char **pder, int derlen)
{
    DSA *dsa;

    if ((dsa = d2i_DSAPrivateKey(NULL, pder, derlen)) == NULL) {
        DSAerr(DSA_F_OLD_DSA_PRIV_DECODE, ERR_R_DSA_LIB);
        return 0;
    }
    EVP_PKEY_assign_DSA(pkey, dsa);
    return 1;
}