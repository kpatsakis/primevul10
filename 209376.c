Jsi_RC Jsi_PrototypeObjSet(Jsi_Interp *interp, const char *key, Jsi_Obj *obj)
{
    Jsi_Value *val;
    Jsi_HashEntry *hPtr = Jsi_HashEntryFind(interp->protoTbl, key);
    if (!hPtr)
        return JSI_ERROR;
    val = (Jsi_Value *)Jsi_HashValueGet(hPtr);
    obj->__proto__ = val;
    return JSI_OK;
}