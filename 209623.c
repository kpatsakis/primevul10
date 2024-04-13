static Jsi_OpCodes *codes_join4(Jsi_OpCodes *a, Jsi_OpCodes *b, Jsi_OpCodes *c, Jsi_OpCodes *d)
{
    return codes_join(codes_join(a, b), codes_join(c, d));
}