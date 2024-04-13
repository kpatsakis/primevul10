int Jsi_Chmod(Jsi_Interp *interp, Jsi_Value* path, int mode) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->chmodProc) return -1;
    return fsPtr->chmodProc(interp, path, mode);
}