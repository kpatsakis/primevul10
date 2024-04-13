int BN_asc2bn(BIGNUM **bn, const char *a)
{
    const char *p = a;
    if (*p == '-')
        p++;

    if (p[0] == '0' && (p[1] == 'X' || p[1] == 'x')) {
        if (!BN_hex2bn(bn, p + 2))
            return 0;
    } else {
        if (!BN_dec2bn(bn, p))
            return 0;
    }
    if (*a == '-')
        (*bn)->neg = 1;
    return 1;
}