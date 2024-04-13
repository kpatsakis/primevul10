Jsi_RC Jsi_PathNormalize(Jsi_Interp *interp, Jsi_Value **pathPtr) {
    Jsi_Value *path = *pathPtr;
    if (!path) {
        Jsi_DString dStr = {};
        *pathPtr = Jsi_ValueNewStringDup(interp, Jsi_GetCwd(interp, &dStr));
        Jsi_IncrRefCount(interp, *pathPtr);
        Jsi_DSFree(&dStr);
    } else {
        const char *rn = Jsi_Realpath(interp, path, NULL);
        if (!rn) return JSI_ERROR;
        Jsi_DecrRefCount(interp, *pathPtr);
        *pathPtr = Jsi_ValueNewString(interp, rn, -1);
        Jsi_IncrRefCount(interp, *pathPtr);
    }
    return JSI_OK;
}