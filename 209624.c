Jsi_RC Jsi_PrototypeDefine(Jsi_Interp *interp, const char *key, Jsi_Value *value)
{
    bool isNew;
    Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->protoTbl, key, &isNew);
    if (!hPtr)
        return JSI_ERROR;
    Jsi_HashValueSet(hPtr, value);
    return JSI_OK;
}