static Jsi_RC freeCodeTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    jsi_Pstate *ps = (jsi_Pstate *)ptr;
    if (!ps) return JSI_OK;
    ps->hPtr = NULL;
    jsi_PstateFree(ps);
    return JSI_OK;
}