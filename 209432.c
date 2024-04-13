static Jsi_RC jsi_wsfreeFile(Jsi_Interp *interp, Jsi_HashEntry* hPtr, void *ptr) {
    jsi_wsFile*h = (jsi_wsFile*)ptr;
    if (!h)
        return JSI_OK;
    if (h->fileVal)
        Jsi_DecrRefCount(interp, h->fileVal);
    Jsi_Free(h);
    return JSI_OK;
}