static Jsi_RC WebSocketHandlerCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    Jsi_HashEntry *hPtr;
    jsi_wsHander *hdlPtr;
    if (!cmdPtr)
        return Jsi_LogError("Apply in a non-websock object");
    WSSIGASSERT(cmdPtr, OBJ);
    int argc = Jsi_ValueGetLength(interp, args);
    if (argc == 0) {
        Jsi_HashSearch search;
        Jsi_Obj* obj = Jsi_ObjNew(interp);
        for (hPtr = Jsi_HashSearchFirst(cmdPtr->handlers, &search); hPtr; hPtr = Jsi_HashSearchNext(&search)) {
            const char *key = (char*)Jsi_HashKeyGet(hPtr);
            Jsi_Value *val = (Jsi_Value*)Jsi_HashValueGet(hPtr);
            Jsi_ObjInsert(interp, obj, key, val, 0);
        }
        Jsi_ValueMakeObject(interp, ret, obj);
        return JSI_OK;
    }
    if (argc == 1) {
        hPtr = Jsi_HashEntryFind(cmdPtr->handlers, Jsi_ValueArrayIndexToStr(interp, args, 0, NULL));
        if (!hPtr)
            return JSI_OK;
        hdlPtr = (jsi_wsHander*)Jsi_HashValueGet(hPtr);
        Jsi_ValueReplace(interp, ret, hdlPtr->val);
        return JSI_OK;
    }
    const char *key = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_Value *valPtr = Jsi_ValueArrayIndex(interp, args, 1);
    if (Jsi_ValueIsNull(interp, valPtr)) {
        hPtr = Jsi_HashEntryFind(cmdPtr->handlers, key);
        if (!hPtr)
            return JSI_OK;
        hdlPtr = (jsi_wsHander*)Jsi_HashValueGet(hPtr);
        if (hdlPtr->val)
            Jsi_DecrRefCount(interp, hdlPtr->val);
        Jsi_HashValueSet(hPtr, NULL);
        Jsi_HashEntryDelete(hPtr);
        Jsi_Free(hdlPtr);
        Jsi_ValueMakeStringDup(interp, ret, key);
        return JSI_OK;
    }
    if (Jsi_ValueIsFunction(interp, valPtr)==0 && Jsi_ValueIsString(interp, valPtr)==0)
        return Jsi_LogError("expected string, function or null");
    Jsi_Value *argPtr = Jsi_ValueArrayIndex(interp, args, 2);
    if (argPtr) {
        if (Jsi_ValueIsNull(interp, argPtr))
            argPtr = NULL;
        else if (!Jsi_ValueIsString(interp, argPtr))
            return Jsi_LogError("expected a string");
    }
    hPtr = Jsi_HashEntryNew(cmdPtr->handlers, key, NULL);
    if (!hPtr)
        return JSI_ERROR;
    hdlPtr = (jsi_wsHander *)Jsi_Calloc(1, sizeof(*hdlPtr));
    Jsi_Value *flagPtr = Jsi_ValueArrayIndex(interp, args, 1);
    Jsi_Number fl = 0;
    if (flagPtr && Jsi_ValueIsNumber(interp, flagPtr))
        Jsi_ValueGetNumber(interp, flagPtr, &fl);
    hdlPtr->val = valPtr;
    hdlPtr->flags = fl;
    Jsi_HashValueSet(hPtr, hdlPtr);
    Jsi_IncrRefCount(interp, valPtr);
    return JSI_OK;
}