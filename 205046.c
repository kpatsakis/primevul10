char *BN_bn2hex(const BIGNUM *a)
{
    int i, j, v, z = 0;
    char *buf;
    char *p;

    if (a->neg && BN_is_zero(a)) {
        /* "-0" == 3 bytes including NULL terminator */
        buf = OPENSSL_malloc(3);
    } else {
        buf = OPENSSL_malloc(a->top * BN_BYTES * 2 + 2);
    }
    if (buf == NULL) {
        BNerr(BN_F_BN_BN2HEX, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    p = buf;
    if (a->neg)
        *(p++) = '-';
    if (BN_is_zero(a))
        *(p++) = '0';
    for (i = a->top - 1; i >= 0; i--) {
        for (j = BN_BITS2 - 8; j >= 0; j -= 8) {
            /* strip leading zeros */
            v = ((int)(a->d[i] >> (long)j)) & 0xff;
            if (z || (v != 0)) {
                *(p++) = Hex[v >> 4];
                *(p++) = Hex[v & 0x0f];
                z = 1;
            }
        }
    }
    *p = '\0';
 err:
    return (buf);
}