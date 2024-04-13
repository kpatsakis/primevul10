jsi_wsgetPss(jsi_wsCmdObj *cmdPtr, struct lws *wsi, void *user, int create, int ishttp)
{
    Jsi_HashEntry *hPtr;
    bool isNew = 0;
    jsi_wsPss *pss = NULL;
    if (user==NULL)
        return NULL;
    int sfd = lws_get_socket_fd(wsi);
    if (sfd<0) {
        return NULL;
    }
    int sid = ((sfd<<1)|ishttp);
    if (create)
        hPtr = Jsi_HashEntryNew(cmdPtr->pssTable, (void*)(intptr_t)sid, &isNew);
    else
        hPtr = Jsi_HashEntryFind(cmdPtr->pssTable, (void*)(intptr_t)sid);

    if (hPtr && !isNew)
        pss = (typeof(pss))Jsi_HashValueGet(hPtr);

    if (!pss) {
        if (!create)
            return NULL;
        pss = (typeof(pss))Jsi_Calloc(1, sizeof(*pss));
        Jsi_HashValueSet(hPtr, pss);
        pss->sig = JWS_SIG_PWS;
        pss->hPtr = hPtr;
        Jsi_HashValueSet(hPtr, pss);
        pss->cmdPtr = cmdPtr;
        pss->wsi = wsi;
        pss->user = user; /* unused. */
        pss->state = PWS_CONNECTED;
        pss->stats.createTime = time(NULL);
        pss->cnt = cmdPtr->idx++;
        pss->wid = sid;
        //pss->sfd = sfd;
        pss->udata = Jsi_ValueNewObj(cmdPtr->interp, NULL);
        Jsi_IncrRefCount(cmdPtr->interp, pss->udata);

        if (cmdPtr->debug>2)
            fprintf(stderr, "PSS CREATE: %p/%p/%p (http=%d) = %d\n", pss, user, wsi, ishttp, sid);
        if (!ishttp) {
            pss->isWebsock = 1;
            cmdPtr->stats.connectCnt++;
            cmdPtr->createCnt++;
            cmdPtr->createLast = time(NULL);
        }
    }
    if (pss) {
        WSSIGASSERT(pss, PWS);
        pss->stats.eventCnt++;
        pss->stats.eventLast = time(NULL);
        cmdPtr->stats.eventCnt++;
        cmdPtr->stats.eventLast = time(NULL);
    }
    return pss;
}