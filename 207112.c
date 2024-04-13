static BIGNUM *set_signed_bn(int value)
{
    BIGNUM *bn = BN_new();

    if (bn == NULL)
        return NULL;
    if (!BN_set_word(bn, value < 0 ? -value : value)) {
        BN_free(bn);
        return NULL;
    }
    BN_set_negative(bn, value < 0);
    return bn;
}