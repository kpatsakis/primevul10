static Jsi_RC regExpFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_RegExpFree((Jsi_Regex*)ptr);
    return JSI_OK;
}