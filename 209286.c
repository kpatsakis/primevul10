int Jsi_Stat(Jsi_Interp *interp, Jsi_Value* path, Jsi_StatBuf *buf) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->statProc) return -1;
    return fsPtr->statProc(interp, path, buf);
}