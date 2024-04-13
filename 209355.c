static Jsi_RC jsi_wsfreePss(Jsi_Interp *interp, Jsi_HashEntry* hPtr, void *ptr) {
    jsi_wsPss *pss = (jsi_wsPss*)ptr;
    WSSIGASSERT(pss, PWS);
    if (pss) {
        pss->hPtr = NULL;
        jsi_wsdeletePss(pss);
    }
    return JSI_OK;
}