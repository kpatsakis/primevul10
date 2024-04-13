static int dsa_param_encode(const EVP_PKEY *pkey, unsigned char **pder)
{
    return i2d_DSAparams(pkey->pkey.dsa, pder);
}