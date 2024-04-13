static Jsi_RC jsi_wsDelPss(Jsi_Interp *interp, void *data) {
    Jsi_Free(data);
    return JSI_OK;
}