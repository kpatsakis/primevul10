static int jsi_FSReadlinkProc(Jsi_Interp *interp, Jsi_Value *path, char *buf, int size) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = readlink(pathPtr, buf, size);
    Jsi_DSFree(&dStr);
    return rc;
}