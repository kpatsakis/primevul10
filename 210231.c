Jsi_RC Jsi_ThisDataSet(Jsi_Interp *interp, Jsi_Value *_this, void *value)
{
    bool isNew;
    Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->thisTbl, _this, &isNew);
    if (!hPtr)
        return JSI_ERROR;
    Jsi_HashValueSet(hPtr, value);
    return JSI_OK;
}