static int parsedecBN(BIGNUM **out, const char *in)
{
    *out = NULL;
    return BN_dec2bn(out, in);
}