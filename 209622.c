jsi_wsdeletePss(jsi_wsPss *pss)
{
    jsi_wsCmdObj*cmdPtr = pss->cmdPtr;
    if (pss->sig == 0)
        return;
    WSSIGASSERT(pss, PWS);
    if (pss->state == PWS_DEAD)
        return;
    if (cmdPtr && cmdPtr->debug>3)
        fprintf(stderr, "PSS DELETE: %p\n", pss);

    jsi_wsrecv_flush(cmdPtr, pss);
    if (pss->hPtr) {
        Jsi_HashValueSet(pss->hPtr, NULL);
        Jsi_HashEntryDelete(pss->hPtr);
        pss->hPtr = NULL;
    }
    Jsi_Interp *interp = cmdPtr->interp;
    if (pss->stack) {
        Jsi_StackFreeElements(interp, pss->stack, jsi_wsDelPss);
        Jsi_StackFree(pss->stack);
    }
    Jsi_DSFree(&pss->dHdrs);
    if (pss->isWebsock)
        pss->cmdPtr->stats.connectCnt--;
    Jsi_OptionsFree(cmdPtr->interp, WPSOptions, pss, 0);
    pss->state = PWS_DEAD;
    Jsi_DSFree(&pss->resultStr);
    Jsi_DSFree(&pss->paramDS);
    if (pss->lastData)
        Jsi_Free(pss->lastData);
    pss->lastData = NULL;
    if (pss->spa)
        lws_spa_destroy(pss->spa);
    Jsi_Free(pss);
}