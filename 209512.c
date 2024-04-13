static char* jsi_FSRealPathProc(Jsi_Interp *interp, Jsi_Value *src, char *newPath) {
    return Jsi_FileRealpath(interp, src, newPath);
}