static int jsi_FSRenameProc(Jsi_Interp *interp, Jsi_Value *src, Jsi_Value *dest) {
    const char *zSrc = Jsi_ValueToString(interp, src, NULL);
    const char *zDest = Jsi_ValueToString(interp, dest, NULL);
    Jsi_DString dStr = {}, eStr = {};
    if (*zSrc == '~')
        zSrc = jsi_TildePath(interp, zSrc, &dStr);
    if (*zDest == '~')
        zDest = jsi_TildePath(interp, zDest, &eStr);
    int rc = rename(zSrc, zDest);
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&eStr);
    return rc;
}