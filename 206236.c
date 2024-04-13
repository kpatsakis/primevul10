int GENERAL_NAME_get0_otherName(const GENERAL_NAME *gen,
                                ASN1_OBJECT **poid, ASN1_TYPE **pvalue)
{
    if (gen->type != GEN_OTHERNAME)
        return 0;
    if (poid)
        *poid = gen->d.otherName->type_id;
    if (pvalue)
        *pvalue = gen->d.otherName->value;
    return 1;
}