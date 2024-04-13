static int jsi_FSChmodProc(Jsi_Interp *interp, Jsi_Value* path, int mode) {
#ifdef __WIN32
    return -1;
#else
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = chmod(pathPtr, mode);
    Jsi_DSFree(&dStr);
    return rc;
#endif
}