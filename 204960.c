static int int_dsa_size(const EVP_PKEY *pkey)
{
    return (DSA_size(pkey->pkey.dsa));
}