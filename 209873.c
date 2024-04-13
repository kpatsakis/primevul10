static int jsi_wsService(jsi_wsCmdObj *cmdPtr)
{
    int n = 0;
    struct timeval tv;
    if (cmdPtr->inUpdate) return 0;
    cmdPtr->inUpdate = 1;

    gettimeofday(&tv, NULL);
    int to = cmdPtr->recvBufTimeout;
    if (to>0 && cmdPtr->recvBufCnt) { // Flush buffered data.
        jsi_wsPss *pss = NULL;
        Jsi_HashEntry *hPtr;
        Jsi_HashSearch cursor;
        for (hPtr = Jsi_HashSearchFirst(cmdPtr->pssTable, &cursor);
            hPtr != NULL; hPtr = Jsi_HashSearchNext(&cursor)) {
            pss = (jsi_wsPss*)Jsi_HashValueGet(hPtr);
            WSSIGASSERT(pss, PWS);
            if (pss->state == PWS_DEAD) continue;
            if (Jsi_DSLength(&pss->recvBuf)<=0) continue;
            if (pss->stats.recvLast && difftime(time(NULL), pss->stats.recvLast)<(double)to) continue;
            jsi_wsrecv_flush(cmdPtr, pss);
        }
    }

    /*
     * This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
     * live websocket connection using the DUMB_INCREMENT protocol,
     * as soon as it can take more packets (usually immediately)
     */

    if (((unsigned int)tv.tv_usec - cmdPtr->oldus) > 50000) {
        lws_callback_on_writable_all_protocol(cmdPtr->context, &cmdPtr->protocols[JWS_PROTOCOL_WEBSOCK]);
        cmdPtr->oldus = tv.tv_usec;
    }

#ifdef EXTERNAL_POLL

    /*
     * this represents an existing server's single poll action
     * which also includes libwebsocket sockets
     */

    n = poll(jsi_wspollfds, jsi_wsnum_pollfds, 50);
    if (n < 0) {
        n = 0;
        goto done;
    }

    if (n)
        for (n = 0; n < jsi_wsnum_pollfds; n++)
            if (jsi_wspollfds[n].revents)
                /*
                * returns immediately if the fd does not
                * match anything under libwebsockets
                * control
                */
                if (lws_service_fd(context, &jsi_wspollfds[n]) < 0) {
                    n = -1;
                    goto done;
                }
done:
#else
    n = lws_service(cmdPtr->context, 50);
#endif
    if (cmdPtr->onModify) {
        jsi_wsOnModify(cmdPtr);
    }
    cmdPtr->inUpdate = 0;
    return n;
}