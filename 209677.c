static Jsi_OpCodes *code_stry(jsi_Pstate *p, jsi_Pline *line, int trylen, int catchlen, int finlen)
{ 
    jsi_TryInfo *ti = (jsi_TryInfo *)Jsi_Calloc(1,sizeof(*ti));
    ti->trylen = trylen;
    ti->catchlen = catchlen;
    ti->finallen = finlen;
    JSI_NEW_CODESLN(1,OP_STRY, ti); 
}