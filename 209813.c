static int jsi_FSAccessProc(Jsi_Interp *interp, Jsi_Value* path, int mode) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = access(pathPtr, mode);
    Jsi_DSFree(&dStr);
    return rc;
}