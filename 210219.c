static int jsi_FSStatProc(Jsi_Interp *interp, Jsi_Value* path, Jsi_StatBuf *buf) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = stat(pathPtr, buf);
    Jsi_DSFree(&dStr);
    return rc;
}