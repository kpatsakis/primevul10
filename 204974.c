static void int_dsa_free(EVP_PKEY *pkey)
{
    DSA_free(pkey->pkey.dsa);
}