static int parse_bigBN(BIGNUM **out, const char *bn_strings[])
{
    char *bigstring = glue_strings(bn_strings, NULL);
    int ret = BN_hex2bn(out, bigstring);

    OPENSSL_free(bigstring);
    return ret;
}