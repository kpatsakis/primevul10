static int jsi_FSLstatProc(Jsi_Interp *interp, Jsi_Value* path, Jsi_StatBuf *buf) {
#ifdef __WIN32
    return jsi_FSStatProc(interp, path, buf);
#else
    const char *pathPtr = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*pathPtr == '~')
        pathPtr = jsi_TildePath(interp, pathPtr, &dStr);
    int rc = lstat(pathPtr, buf);
    Jsi_DSFree(&dStr);
    return rc;
#endif
}