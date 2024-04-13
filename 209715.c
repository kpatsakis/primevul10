bool Jsi_FSNative(Jsi_Interp *interp, Jsi_Value *file) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, file, &data);
    if (fsPtr && fsPtr == &jsiFilesystem)
        return 1;
    else
        return 0;
}