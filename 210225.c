static Jsi_RC WebSocketConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);

    if (!cmdPtr)
        return Jsi_LogError("Apply in a non-websock object");
    Jsi_Value *opts = Jsi_ValueArrayIndex(interp, args, 0);
    if (cmdPtr->noConfig && opts && !Jsi_ValueIsString(interp, opts))
        return Jsi_LogError("WebSocket conf() is disabled for set");
    return Jsi_OptionsConf(interp, WSOptions, cmdPtr, opts, ret, 0);

}