static Jsi_RC freeUserdataTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    if (ptr)
        jsi_UserObjDelete(interp, ptr);
    return JSI_OK;
}