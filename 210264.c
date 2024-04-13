int Jsi_Rename(Jsi_Interp *interp, Jsi_Value *src, Jsi_Value *dst) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, src, &data);
    if (fsPtr != Jsi_FilesystemForPath(interp, src, &data)) return -1;
    if (fsPtr == NULL || !fsPtr->renameProc) return -1;
    return fsPtr->renameProc(interp, src,dst);
}