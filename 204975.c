static int dsa_bits(const EVP_PKEY *pkey)
{
    return BN_num_bits(pkey->pkey.dsa->p);
}