static SRP_gN *SRP_get_gN_by_id(const char *id, STACK_OF(SRP_gN) *gN_tab)
{
    int i;

    SRP_gN *gN;
    if (gN_tab != NULL)
        for (i = 0; i < sk_SRP_gN_num(gN_tab); i++) {
            gN = sk_SRP_gN_value(gN_tab, i);
            if (gN && (id == NULL || strcmp(gN->id, id) == 0))
                return gN;
        }

    return SRP_get_default_gN(id);
}