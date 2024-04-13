Jsi_RC Jsi_FSUnregister(Jsi_Filesystem *fsPtr) {
    FSList *fsl = jsiFSList, *flast = NULL;
    while (fsl) {
        if (fsl->fsPtr == fsPtr) {
            if (flast)
                flast->next = fsl->next;
            else
                jsiFSList = fsl->next;
            Jsi_Free(fsl);
            break;
        }
        flast = fsl;
        fsl = fsl->next;
    }
    return JSI_OK;
}