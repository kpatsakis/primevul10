int Jsi_Access(Jsi_Interp *interp, Jsi_Value* path, int mode) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->accessProc) return -1;
    if (interp->isSafe && fsPtr && fsPtr == &jsiFilesystem) {
        int aflag = (mode&W_OK ? JSI_INTACCESS_WRITE : JSI_INTACCESS_READ);
        if (Jsi_InterpAccess(interp, path, aflag) != JSI_OK)
            return -1;
    }
    return fsPtr->accessProc(interp, path, mode);
}