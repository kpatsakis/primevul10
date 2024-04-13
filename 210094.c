void Jsi_InterpSetData(Jsi_Interp *interp, const char *key, void *data, Jsi_DeleteProc *proc)
{
    bool isNew;
    Jsi_HashEntry *hPtr;
    AssocData *ptr;
    hPtr = Jsi_HashEntryNew(interp->assocTbl, key, &isNew);
    if (!hPtr)
        return;
    if (isNew) {
        ptr = (AssocData *)Jsi_Calloc(1,sizeof(*ptr));
        Jsi_HashValueSet(hPtr, ptr);
    } else
        ptr = (AssocData *)Jsi_HashValueGet(hPtr);
    ptr->data = data;
    ptr->delProc = proc;
}