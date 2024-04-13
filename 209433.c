static Jsi_RC WebSocketStatusCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply to non-websock object");
#ifndef OMIT_LWS_WITH_SERVER_STATUS
    char cbuf[JSI_BUFSIZ*2];
    lws_json_dump_context(cmdPtr->context, cbuf, sizeof(cbuf), 0);
    return Jsi_JSONParse(interp, cbuf, ret, 0);
#else
    return Jsi_LogError("unsupported");
#endif
}