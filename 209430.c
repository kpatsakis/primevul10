Jsi_RC Jsi_FSRegister(Jsi_Filesystem *fsPtr, void *data) {
    FSList *fsl = (FSList *)Jsi_Calloc(1, sizeof(*fsl));
    fsl->fsPtr = fsPtr;
    fsl->data = data;
    fsl->next = jsiFSList;
    jsiFSList = fsl;
    return JSI_OK;
}