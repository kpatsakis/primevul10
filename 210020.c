Jsi_RC Jsi_InitWebSocket(Jsi_Interp *interp, int release)
{
    if (release)
        return Jsi_DoneWebSocket(interp);
#ifdef LWS_OPENSSL_SUPPORT
    Jsi_InterpAccess(interp, NULL, JSI_INTACCESS_SETSSL );
#endif
    Jsi_Hash *wsys;
    const char *libver = lws_get_library_version();
    int lvlen = sizeof(LWS_LIBRARY_VERSION)-1;
    if (Jsi_Strncmp(libver, LWS_LIBRARY_VERSION, lvlen) || !isspace(libver[lvlen]))
        return Jsi_LogError("Library version mismatch: HDR:%s != LIB:%s", LWS_LIBRARY_VERSION, libver);
#if JSI_USE_STUBS
  if (Jsi_StubsInit(interp, 0) != JSI_OK)
    return JSI_ERROR;
#endif
    Jsi_Value *info = Jsi_ValueNew1(interp);
    Jsi_JSONParseFmt(interp, &info, "{libVer:\"%s\", hdrVer:\"%s\", pkgVer:%d}",
        libver, LWS_LIBRARY_VERSION, jsi_WsPkgVersion);
    Jsi_PkgOpts wsPkgOpts = { wsObjCmd_Specs, &wsObjCmd, websockCmds, info };
    Jsi_RC rc = Jsi_PkgProvideEx(interp, "WebSocket", jsi_WsPkgVersion, Jsi_InitWebSocket, &wsPkgOpts);
    Jsi_DecrRefCount(interp, info);
    if (rc != JSI_OK)
        return JSI_ERROR;
    if (!(wsys = Jsi_UserObjRegister(interp, &websockobject))) {
        Jsi_LogBug("Can not init webSocket");
        return JSI_ERROR;
    }

    if (!Jsi_CommandCreateSpecs(interp, websockobject.name, websockCmds, wsys, JSI_CMDSPEC_ISOBJ))
        return JSI_ERROR;
    return JSI_OK;
}