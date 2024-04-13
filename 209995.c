static int jsi_FSRemoveProc(Jsi_Interp *interp, Jsi_Value* path, int flags) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = remove(pathPtr);
    Jsi_DSFree(&dStr);
    return rc;
}