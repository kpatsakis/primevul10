void *Jsi_ThisDataGet(Jsi_Interp *interp, Jsi_Value *_this)
{
    Jsi_HashEntry *hPtr;
    hPtr = Jsi_HashEntryFind(interp->thisTbl, _this);
    if (!hPtr)
        return NULL;
    return Jsi_HashValueGet(hPtr);
}