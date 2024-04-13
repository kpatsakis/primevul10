static Jsi_RC jsi_wsrecv_flush(jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss)
{
    int nlen = Jsi_DSLength(&pss->recvBuf);
    if (nlen<=0)
        return JSI_OK;
    cmdPtr->recvBufCnt--;
    const char *inPtr = Jsi_DSFreeDup(&pss->recvBuf);
    Jsi_RC rc = jsi_wsrecv_callback(cmdPtr->interp, cmdPtr, pss, inPtr, nlen, 0);
    if (rc != JSI_OK) {
        pss->stats.recvErrCnt++;
        pss->stats.recvErrLast = time(NULL);
    }
    return rc;
}