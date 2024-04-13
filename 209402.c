void Jsi_InterpFreeData(Jsi_Interp *interp, const char *key)
{
    Jsi_HashEntry *hPtr;
    hPtr = Jsi_HashEntryFind(interp->assocTbl, key);
    if (!hPtr)
        return;
    Jsi_HashEntryDelete(hPtr);
}