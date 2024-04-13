static Jsi_OpCodes *codes_join3(Jsi_OpCodes *a, Jsi_OpCodes *b, Jsi_OpCodes *c)
{
    return codes_join(codes_join(a, b), c);
}