static void SRP_gN_free(SRP_gN_cache *gN_cache)
{
    if (gN_cache == NULL)
        return;
    OPENSSL_free(gN_cache->b64_bn);
    BN_free(gN_cache->bn);
    OPENSSL_free(gN_cache);
}