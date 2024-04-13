static Jsi_RC freeAssocTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    if (!ptr) return JSI_OK;
    jsi_DelAssocData(interp, ptr);
    return JSI_OK;
}