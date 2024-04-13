int jsi_PstateSetString(jsi_Pstate *ps, const char *str)
{
    Jsi_Interp *interp = ps->interp;
    jsi_Lexer *l = ps->lexer;
    jsi_PstateClear(ps);
    l->ltype = LT_STRING;
    Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->codeTbl, (void*)str, NULL);
    assert(hPtr);
    l->d.str = (char*)Jsi_HashKeyGet(hPtr);
    return JSI_OK;
}