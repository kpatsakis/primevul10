int Jsi_Remove(Jsi_Interp *interp, Jsi_Value* path, int flags) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->removeProc) return -1;
    return fsPtr->removeProc(interp, path, flags);
}