static int jsi_wswrite(jsi_wsPss* pss, struct lws *wsi, unsigned char *buf, int len, enum lws_write_protocol proto)
{
    jsi_wsCmdObj *cmdPtr = pss->cmdPtr;
    int m = lws_write(wsi, buf, len, proto);
    if (m >= 0) {
        cmdPtr->stats.sentCnt++;
        cmdPtr->stats.sentLast = time(NULL);
        pss->stats.sentCnt++;
        pss->stats.sentLast = time(NULL);
    } else {
        pss->state = PWS_SENDERR;
        pss->stats.sentErrCnt++;
        pss->stats.sentErrLast = time(NULL);
        cmdPtr->stats.sentErrCnt++;
        cmdPtr->stats.sentErrLast = time(NULL);
    }
    return m;
}