static Jsi_RC freeCmdSpecTbl(Jsi_Interp *interp, Jsi_MapEntry *hPtr, void *ptr) {
    if (!ptr) return JSI_OK;
    jsi_CmdSpecDelete(interp, ptr);
    return JSI_OK;
}