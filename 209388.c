void *Jsi_PrototypeGet(Jsi_Interp *interp, const char *key)
{
    Jsi_HashEntry *hPtr = Jsi_HashEntryFind(interp->protoTbl, key);
    if (!hPtr)
        return NULL;
    return Jsi_HashValueGet(hPtr);
}