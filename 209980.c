int Jsi_Chdir(Jsi_Interp *interp, Jsi_Value* path) {
    void *data;
    int rc = 0;
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    if (interp->isSafe && Jsi_InterpAccess(interp, path, JSI_INTACCESS_READ) != JSI_OK) 
        return Jsi_LogError("read access denied");
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == &jsiFilesystem) {
        rc = chdir(pathPtr);
        if (rc < 0)
            return -1;
        /* If change out of native fs, GetCwd will use pwdStr */
        fsPtr = NULL;
    }
    Jsi_DSSetLength(&jsiIntData.pwdStr, 0);
    Jsi_DSAppendLen(&jsiIntData.pwdStr, pathPtr, -1);
    jsiIntData.cwdFsPtr = fsPtr;
    jsiIntData.pwd = fsPtr ? Jsi_DSValue(&jsiIntData.pwdStr) : NULL;
    return rc;
}