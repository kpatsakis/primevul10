static int old_dsa_priv_encode(const EVP_PKEY *pkey, unsigned char **pder)
{
    return i2d_DSAPrivateKey(pkey->pkey.dsa, pder);
}