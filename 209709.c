int Jsi_Link(Jsi_Interp *interp, Jsi_Value* src, Jsi_Value *dest, int typ) {
    void *data;
    Jsi_Filesystem *fsPtr = Jsi_FilesystemForPath(interp, src, &data);
    if (fsPtr == NULL || !fsPtr->linkProc) return -1;
    return fsPtr->linkProc(interp, src, dest, typ);
}