static Jsi_RC Jsi_DoneWebSocket(Jsi_Interp *interp)
{
    Jsi_UserObjUnregister(interp, &websockobject);
    Jsi_PkgProvide(interp, "WebSocket", -1, NULL);
    return JSI_OK;
}