static jsi_Pstate* jsiNewParser(Jsi_Interp* interp, const char *codeStr, Jsi_Channel fp, int iseval)
{
    bool isNew;
    Jsi_HashEntry *hPtr = NULL;
    hPtr = Jsi_HashEntryNew(interp->codeTbl, (void*)codeStr, &isNew);
    if (!hPtr) return NULL;
    jsi_Pstate *ps, *topPs;

    if (isNew==0 && ((ps = (jsi_Pstate *)Jsi_HashValueGet(hPtr)))) {
        interp->codeCacheHit++;
        return ps;
    }
    ps = jsi_PstateNew(interp);
    ps->eval_flag = iseval;
    if (codeStr)
        jsi_PstateSetString(ps, codeStr);
    else
        jsi_PstateSetFile(ps, fp, 1);
        
    interp->inParse++;
    topPs = interp->parsePs;
    interp->parsePs = ps;
    yyparse(ps);
    interp->parsePs = topPs;
    interp->inParse--;
    
    if (ps->err_count) {
        Jsi_HashEntryDelete(hPtr);
        jsi_PstateFree(ps);
        return NULL;
    }
    if (isNew) {
        Jsi_HashValueSet(hPtr, ps);
        ps->hPtr = hPtr;
    }
    return ps;
}