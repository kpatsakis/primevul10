char * Jsi_GetCwd(Jsi_Interp *interp, Jsi_DString *cwdPtr) {
    char cdbuf[PATH_MAX];
    Jsi_DSInit(cwdPtr);
    if (jsiIntData.cwdFsPtr)
        Jsi_DSAppend(cwdPtr, Jsi_DSValue(&jsiIntData.pwdStr), NULL);
    else
        Jsi_DSAppend(cwdPtr, getcwd(cdbuf, sizeof(cdbuf)), NULL);
#ifdef __WIN32
    DeBackSlashify(Jsi_DSValue(cwdPtr));
#endif
    return Jsi_DSValue(cwdPtr);
}