static int dsa_security_bits(const EVP_PKEY *pkey)
{
    return DSA_security_bits(pkey->pkey.dsa);
}