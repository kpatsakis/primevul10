static Jsi_OpCodes *code_reserved(jsi_Pstate *p, jsi_Pline *line, int type, const char *id)
{
    jsi_ReservedInfo *ri = (jsi_ReservedInfo*)Jsi_Calloc(1, sizeof(*ri));
    ri->type = type;
    ri->label = id;
    ri->topop = 0;
    JSI_NEW_CODESLN(1,OP_RESERVED, ri);
}