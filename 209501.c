static Jsi_RC freeOnDeleteTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    if (!ptr) return JSI_OK;
    Jsi_DeleteProc *proc = (Jsi_DeleteProc *)ptr;
    proc(interp, NULL);
    return JSI_OK;
}