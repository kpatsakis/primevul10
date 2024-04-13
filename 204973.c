static int dsa_pub_cmp(const EVP_PKEY *a, const EVP_PKEY *b)
{
    if (BN_cmp(b->pkey.dsa->pub_key, a->pkey.dsa->pub_key) != 0)
        return 0;
    else
        return 1;
}