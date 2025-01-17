int BN_dec2bn(BIGNUM **bn, const char *a)
{
    BIGNUM *ret = NULL;
    BN_ULONG l = 0;
    int neg = 0, i, j;
    int num;

    if ((a == NULL) || (*a == '\0'))
        return (0);
    if (*a == '-') {
        neg = 1;
        a++;
    }

    for (i = 0; i <= (INT_MAX/4) && isdigit((unsigned char)a[i]); i++)
        continue;

    if (i == 0 || i > INT_MAX/4)
        goto err;

    num = i + neg;
    if (bn == NULL)
        return (num);

    /*
     * a is the start of the digits, and it is 'i' long. We chop it into
     * BN_DEC_NUM digits at a time
     */
    if (*bn == NULL) {
        if ((ret = BN_new()) == NULL)
            return (0);
    } else {
        ret = *bn;
        BN_zero(ret);
    }

    /* i is the number of digits, a bit of an over expand */
    if (bn_expand(ret, i * 4) == NULL)
        goto err;

    j = BN_DEC_NUM - (i % BN_DEC_NUM);
    if (j == BN_DEC_NUM)
        j = 0;
    l = 0;
    while (*a) {
        l *= 10;
        l += *a - '0';
        a++;
        if (++j == BN_DEC_NUM) {
            if (!BN_mul_word(ret, BN_DEC_CONV)
                || !BN_add_word(ret, l))
                goto err;
            l = 0;
            j = 0;
        }
    }
    ret->neg = neg;

    bn_correct_top(ret);
    *bn = ret;
    bn_check_top(ret);
    return (num);
 err:
    if (*bn == NULL)
        BN_free(ret);
    return (0);
}