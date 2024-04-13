static Jsi_RC jsi_wswebsockUpdate(Jsi_Interp *interp, void *data)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)data;
    WSSIGASSERT(cmdPtr,OBJ);
    jsi_wsService(cmdPtr);
    return JSI_OK;
}