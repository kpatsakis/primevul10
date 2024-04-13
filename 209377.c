static Jsi_RC jsi_wsHandlerAdd(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, const char *ext, const char *cmd, int flags)
{
    Jsi_HashEntry *hPtr;
    jsi_wsHander *hdlPtr;
    Jsi_Value *valPtr = Jsi_ValueNewStringDup(interp, cmd);
    hPtr = Jsi_HashEntryNew(cmdPtr->handlers, ext, NULL);
    if (!hPtr)
        return JSI_ERROR;
    hdlPtr = (jsi_wsHander *)Jsi_Calloc(1, sizeof(*hdlPtr));
    hdlPtr->val = valPtr;
    hdlPtr->flags = flags;
    Jsi_HashValueSet(hPtr, hdlPtr);
    Jsi_IncrRefCount(interp, valPtr);
    return JSI_OK;
}