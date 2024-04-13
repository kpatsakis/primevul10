static Jsi_RC packageHashFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    jsi_PkgInfo *p = (jsi_PkgInfo*)ptr;
    if (p->popts.info) Jsi_DecrRefCount(interp, p->popts.info);
    Jsi_Free(p);
    return JSI_OK;
}