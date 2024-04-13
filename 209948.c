static int jsi_FSCreateDirectoryProc(Jsi_Interp *interp, Jsi_Value* path) {
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    int rc;
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);

#ifdef __WIN32
    rc = mkdir(pathPtr);
#else
    rc = mkdir(pathPtr, 0666);
#endif
    Jsi_DSFree(&dStr);
    return rc;
}