static Jsi_RC WebSocketUpdateCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply to non-websock object");
    if (!cmdPtr->noUpdate)
        jsi_wsService(cmdPtr);
    return JSI_OK;
}