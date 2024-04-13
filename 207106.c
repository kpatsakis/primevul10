static int parseBN(BIGNUM **out, const char *in)
{
    *out = NULL;
    return BN_hex2bn(out, in);
}