jsi_Pstate *jsi_PstateNew(Jsi_Interp *interp)
{
    jsi_Pstate *ps = (jsi_Pstate *)Jsi_Calloc(1,sizeof(*ps));
    SIGINIT(ps,PARSER);
    ps->lexer = (jsi_Lexer*)Jsi_Calloc(1,sizeof(*ps->lexer));
    ps->lexer->pstate = ps;
    ps->interp = interp;
    ps->fastVarTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, NULL /*fastVarFree*/);
    ps->strTbl = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, jsi_StringFree);
    return ps;
}