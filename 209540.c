void *Jsi_InterpGetData(Jsi_Interp *interp, const char *key, Jsi_DeleteProc **proc)
{
    Jsi_HashEntry *hPtr;
    AssocData *ptr;
    hPtr = Jsi_HashEntryFind(interp->assocTbl, key);
    if (!hPtr)
        return NULL;
    ptr = (AssocData *)Jsi_HashValueGet(hPtr);
    if (!ptr)
        return NULL;
    if (proc)
        *proc = ptr->delProc;
    return ptr->data;
}