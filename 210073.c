Jsi_RC jsi_HashFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Free(ptr);
    return JSI_OK;
}