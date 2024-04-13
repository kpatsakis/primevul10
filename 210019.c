static Jsi_RC WebSocketFileCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply to non-websock object");
    Jsi_Value *val = Jsi_ValueArrayIndex(interp, args, 0);
    if (val)
        return jsi_wsFileAdd(interp, cmdPtr, val);
    if (cmdPtr->fileHash)
        return Jsi_HashKeysDump(interp, cmdPtr->fileHash, ret, 0);
    return JSI_OK;
}