Jsi_Filesystem* Jsi_FilesystemForPath(Jsi_Interp *interp, Jsi_Value* path, void**clientDataPtr) {
    FSList *fsl = jsiFSList;
    if (!fsl) return NULL;
    clientDataPtr = NULL;
    const char *pathStr = Jsi_ValueToString(interp, path, NULL);
    if (!pathStr || pathStr[0] == '~')
        return &jsiFilesystem;
    if (pathStr[0] == '.' && pathStr[1] == 0)
        return (jsiIntData.cwdFsPtr ? jsiIntData.cwdFsPtr : &jsiFilesystem);
    Jsi_Value *tpath = NULL;
    if (Jsi_Strstr(pathStr, "..")) {
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        pathStr = Jsi_ValueNormalPath(interp, path, &dStr);
        tpath = path = Jsi_ValueNewStringDup(interp, pathStr);
        Jsi_IncrRefCount(interp, tpath);
        Jsi_DSFree(&dStr);
    }
    while (1) {
        if (fsl->fsPtr->pathInFilesystemProc && fsl->fsPtr->pathInFilesystemProc(interp, path, clientDataPtr))
            break;
        if (!fsl->next)
            break;
        fsl = fsl->next;
    }
    if (tpath)
        Jsi_DecrRefCount(interp, tpath);
    return (fsl ? fsl->fsPtr : &jsiFilesystem);
}