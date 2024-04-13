static int jsi_FSLinkProc(Jsi_Interp *interp, Jsi_Value* path, Jsi_Value *toPath, int linkType) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    const char *toPtr = Jsi_ValueToString(interp, toPath, NULL);
    Jsi_DString dStr = {}, eStr = {};
    int rc;
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    if (*toPtr == '~')
        toPtr = jsi_TildePath(interp, toPtr, &eStr);
    if (linkType != 0)
        rc = link(pathPtr, toPtr);
    else
        rc = symlink(pathPtr, toPtr);
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&eStr);
    return rc;
}