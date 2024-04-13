static Jsi_RC jsi_wsfreeHandlers(Jsi_Interp *interp, Jsi_HashEntry* hPtr, void *ptr) {
    jsi_wsHander *h = (jsi_wsHander*)ptr;
    if (!h)
        return JSI_OK;
    if (h->val)
        Jsi_DecrRefCount(interp, h->val);
    if (h->objVar)
        Jsi_DecrRefCount(interp, h->objVar);
    Jsi_Free(h);
    return JSI_OK;
}