WriterListObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int          result = TCL_OK;
    NsServer    *servPtr = NULL;
    Ns_ObjvSpec  lopts[] = {
        {"-server",  Ns_ObjvServer,    &servPtr, NULL},
        {NULL, NULL, NULL, NULL}
    };

    if (unlikely(Ns_ParseObjv(lopts, NULL, interp, 2, objc, objv) != NS_OK)) {
        result = TCL_ERROR;

    } else {
        Tcl_DString       ds, *dsPtr = &ds;
        const Driver     *drvPtr;
        SpoolerQueue     *queuePtr;

        Tcl_DStringInit(dsPtr);

        for (drvPtr = firstDrvPtr; drvPtr != NULL; drvPtr = drvPtr->nextPtr) {
            const DrvWriter *wrPtr;

            /*
             * If server was specified, list only results from this server.
             */
            if (servPtr != NULL && servPtr != drvPtr->servPtr) {
                continue;
            }

            wrPtr = &drvPtr->writer;
            queuePtr = wrPtr->firstPtr;
            while (queuePtr != NULL) {
                const WriterSock *wrSockPtr;

                Ns_MutexLock(&queuePtr->lock);
                wrSockPtr = queuePtr->curPtr;
                while (wrSockPtr != NULL) {
                    char ipString[NS_IPADDR_SIZE];
                    ns_inet_ntop((struct sockaddr *)&(wrSockPtr->sockPtr->sa),
                                 ipString,sizeof(ipString));

                    (void) Ns_DStringNAppend(dsPtr, "{", 1);
                    (void) Ns_DStringAppendTime(dsPtr, &wrSockPtr->startTime);
                    (void) Ns_DStringNAppend(dsPtr, " ", 1);
                    (void) Ns_DStringAppend(dsPtr, queuePtr->threadName);
                    (void) Ns_DStringNAppend(dsPtr, " ", 1);
                    (void) Ns_DStringAppend(dsPtr, drvPtr->threadName);
                    (void) Ns_DStringNAppend(dsPtr, " ", 1);
                    (void) Ns_DStringAppend(dsPtr, NsPoolName(wrSockPtr->poolPtr->pool));
                    (void) Ns_DStringNAppend(dsPtr, " ", 1);
                    (void) Ns_DStringAppend(dsPtr, ipString);
                    (void) Ns_DStringPrintf(dsPtr, " %d %" PRIdz " %" TCL_LL_MODIFIER "d %d %d ",
                                            wrSockPtr->fd,
                                            wrSockPtr->size,
                                            wrSockPtr->nsent,
                                            wrSockPtr->currentRate,
                                            wrSockPtr->rateLimit);
                    (void) Ns_DStringAppendElement(dsPtr,
                                                   (wrSockPtr->clientData != NULL)
                                                   ? wrSockPtr->clientData
                                                   : NS_EMPTY_STRING);
                    (void) Ns_DStringNAppend(dsPtr, "} ", 2);
                    wrSockPtr = wrSockPtr->nextPtr;
                }
                Ns_MutexUnlock(&queuePtr->lock);
                queuePtr = queuePtr->nextPtr;
            }
        }
        Tcl_DStringResult(interp, &ds);
    }
    return result;
}