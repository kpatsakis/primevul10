int Jsi_Readlink(Jsi_Interp *interp, Jsi_Value* path, char *ret, int len) {
#ifdef __WIN32
    return -1;
#else
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, path, &data);
    if (fsPtr == NULL || !fsPtr->readlinkProc) return -1;
    return fsPtr->readlinkProc(interp, path, ret, len);
#endif
}