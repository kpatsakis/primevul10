DriverThread(void *arg)
{
    Driver        *drvPtr = (Driver*)arg;
    Ns_Time        now, diff;
    char           charBuffer[1], drain[1024];
    int            pollTimeout, accepted, nrBindaddrs = 0;
    bool           stopping;
    unsigned int   flags;
    Sock          *sockPtr, *closePtr, *nextPtr, *waitPtr, *readPtr;
    PollData       pdata;

    Ns_ThreadSetName("-driver:%s-", drvPtr->threadName);
    Ns_Log(Notice, "starting");

    flags = DRIVER_STARTED;

    {
        Tcl_Obj *bindaddrsObj, **objv;
        int      j = 0, result;

        bindaddrsObj = Tcl_NewStringObj(drvPtr->address, -1);
        Tcl_IncrRefCount(bindaddrsObj);

        result = Tcl_ListObjGetElements(NULL, bindaddrsObj, &nrBindaddrs, &objv);
        /*
         * "result" was ok during startup, it has still to be ok.
         */
        assert(result == TCL_OK);

        if (result == TCL_OK) {
            int i;

            /*
             * Bind all provided addresses.
             */
            for (i = 0; i < nrBindaddrs; i++) {

                drvPtr->listenfd[j] = DriverListen(drvPtr, Tcl_GetString(objv[i]));
                if (drvPtr->listenfd[j] != NS_INVALID_SOCKET) {
                    j ++;
                }
            }
            if (j > 0 && j < nrBindaddrs) {
                Ns_Log(Warning, "could only bind to %d out of %d addresses", j, nrBindaddrs);
            }
        }

        /*
         * "j" refers to the number of successful listen() operations.
         */
        nrBindaddrs = j;
        Tcl_DecrRefCount(bindaddrsObj);
    }

    if (nrBindaddrs > 0) {
        SpoolerQueueStart(drvPtr->spooler.firstPtr, SpoolerThread);
        SpoolerQueueStart(drvPtr->writer.firstPtr, WriterThread);
    } else {
        Ns_Log(Warning, "could no bind any of the following addresses, stopping this driver: %s", drvPtr->address);
        flags |= (DRIVER_FAILED | DRIVER_SHUTDOWN);
    }

    Ns_MutexLock(&drvPtr->lock);
    drvPtr->flags |= flags;
    Ns_CondBroadcast(&drvPtr->cond);
    Ns_MutexUnlock(&drvPtr->lock);

    /*
     * Loop forever until signaled to shut down and all
     * connections are complete and gracefully closed.
     */

    PollCreate(&pdata);
    Ns_GetTime(&now);
    closePtr = waitPtr = readPtr = NULL;
    stopping = ((flags & DRIVER_SHUTDOWN) != 0u);

    if (!stopping) {
        Ns_Log(Notice, "driver: accepting connections");
    }

    while (!stopping) {
        int n;

        /*
         * Set the bits for all active drivers if a connection
         * isn't already pending.
         */

        PollReset(&pdata);
        (void)PollSet(&pdata, drvPtr->trigger[0], (short)POLLIN, NULL);

        if (likely(waitPtr == NULL)) {

            for (n = 0; n < nrBindaddrs; n++) {
                drvPtr->pidx[n] = PollSet(&pdata, drvPtr->listenfd[n],
                                          (short)POLLIN, NULL);
            }
        }

        /*
         * If there are any closing or read-ahead sockets, set the bits
         * and determine the minimum relative timeout.
         *
         * TODO: the various poll timeouts should probably be configurable.
         */

        if (readPtr == NULL && closePtr == NULL) {
            pollTimeout = 10 * 1000;
        } else {

            for (sockPtr = readPtr; sockPtr != NULL; sockPtr = sockPtr->nextPtr) {
                SockPoll(sockPtr, (short)POLLIN, &pdata);
            }
            for (sockPtr = closePtr; sockPtr != NULL; sockPtr = sockPtr->nextPtr) {
                SockPoll(sockPtr, (short)POLLIN, &pdata);
            }

            if (Ns_DiffTime(&pdata.timeout, &now, &diff) > 0)  {
                /*
                 * The resolution of "pollTimeout" is ms, therefore, we round
                 * up. If we would round down (e.g. 500 microseconds to 0 ms),
                 * the time comparison later would determine that it is too
                 * early.
                 */
                pollTimeout = (int)Ns_TimeToMilliseconds(&diff) + 1;

            } else {
                pollTimeout = 0;
            }
        }

        n = PollWait(&pdata, pollTimeout);

        Ns_Log(DriverDebug, "=== PollWait returned %d, trigger[0] %d", n, PollIn(&pdata, 0));

        if (PollIn(&pdata, 0) && unlikely(ns_recv(drvPtr->trigger[0], charBuffer, 1u, 0) != 1)) {
            const char *errstr = ns_sockstrerror(ns_sockerrno);

            Ns_Fatal("driver: trigger ns_recv() failed: %s", errstr);
        }
        /*
         * Check whether we should re-animate some connection threads,
         * when e.g. the number of current threads dropped below the
         * minimal value.  Perform this test on timeouts (n == 0;
         * just for safety reasons) or on explicit wakeup calls.
         */
        if ((n == 0) || PollIn(&pdata, 0)) {
            NsServer *servPtr = drvPtr->servPtr;

            if (servPtr != NULL) {
                /*
                 * Check if we have to reanimate the current server.
                 */
                NsEnsureRunningConnectionThreads(servPtr, NULL);

            } else {
                Ns_Set *servers = Ns_ConfigCreateSection("ns/servers");
                size_t  j;

                /*
                 * Reanimation check on all servers.
                 */
                for (j = 0u; j < Ns_SetSize(servers); ++j) {
                    const char *server  = Ns_SetKey(servers, j);

                    servPtr = NsGetServer(server);
                    if (servPtr != NULL) {
                        NsEnsureRunningConnectionThreads(servPtr, NULL);
                    }
                }
            }
        }

        /*
         * Update the current time and drain and/or release any
         * closing sockets.
         */
        Ns_GetTime(&now);

        if (closePtr != NULL) {
            sockPtr  = closePtr;
            closePtr = NULL;
            while (sockPtr != NULL) {
                nextPtr = sockPtr->nextPtr;
                if (unlikely(PollHup(&pdata, sockPtr->pidx))) {
                    /*
                     * Peer has closed the connection
                     */
                    SockRelease(sockPtr, SOCK_CLOSE, 0);
                } else if (likely(PollIn(&pdata, sockPtr->pidx))) {
                    /*
                     * Got some data
                     */
                    ssize_t received = ns_recv(sockPtr->sock, drain, sizeof(drain), 0);
                    if (received <= 0) {
                        Ns_Log(DriverDebug, "poll closewait pollin; sockrelease SOCK_READERROR (sock %d)",
                               sockPtr->sock);
                        SockRelease(sockPtr, SOCK_READERROR, 0);
                    } else {
                        Push(sockPtr, closePtr);
                    }
                } else if (Ns_DiffTime(&sockPtr->timeout, &now, &diff) <= 0) {
                    /* no PollHup, no PollIn, maybe timeout */
                    Ns_Log(DriverDebug, "poll closewait timeout; sockrelease SOCK_CLOSETIMEOUT (sock %d)",
                           sockPtr->sock);
                    SockRelease(sockPtr, SOCK_CLOSETIMEOUT, 0);
                } else {
                    /* too early, keep waiting */
                    Push(sockPtr, closePtr);
                }
                sockPtr = nextPtr;
            }
        }

        /*
         * Attempt read-ahead of any new connections.
         */

        sockPtr = readPtr;
        readPtr = NULL;

        while (likely(sockPtr != NULL)) {
            nextPtr = sockPtr->nextPtr;

            if (unlikely(PollHup(&pdata, sockPtr->pidx))) {
                /*
                 * Peer has closed the connection
                 */
                SockRelease(sockPtr, SOCK_CLOSE, 0);

            } else if (unlikely(!PollIn(&pdata, sockPtr->pidx))
                       && ((sockPtr->reqPtr == NULL) || (sockPtr->reqPtr->leftover == 0u))) {
                /*
                 * Got no data for this sockPtr.
                 */
                if (Ns_DiffTime(&sockPtr->timeout, &now, &diff) <= 0) {
                    SockRelease(sockPtr, SOCK_READTIMEOUT, 0);
                } else {
                    Push(sockPtr, readPtr);
                }

            } else {
                /*
                 * Got some data for this sockPtr.
                 * If enabled, perform read-ahead now.
                 */
                assert(drvPtr == sockPtr->drvPtr);

                if (likely((drvPtr->opts & NS_DRIVER_ASYNC) != 0u)) {
                    SockState s = SockRead(sockPtr, 0, &now);

                    /*
                     * Queue for connection processing if ready.
                     */

                    switch (s) {
                    case SOCK_SPOOL:
                        drvPtr->stats.spooled++;
                        if (SockSpoolerQueue(drvPtr, sockPtr) == 0) {
                            Push(sockPtr, readPtr);
                        }
                        break;

                    case SOCK_MORE:
                        drvPtr->stats.partial++;
                        SockTimeout(sockPtr, &now, &drvPtr->recvwait);
                        Push(sockPtr, readPtr);
                        break;

                    case SOCK_READY:
                        if (SockQueue(sockPtr, &now) == NS_TIMEOUT) {
                            Push(sockPtr, waitPtr);
                        }
                        break;

                        /*
                         * Already handled or normal cases
                         */
                    case SOCK_ENTITYTOOLARGE:  NS_FALL_THROUGH; /* fall through */
                    case SOCK_BADREQUEST:      NS_FALL_THROUGH; /* fall through */
                    case SOCK_BADHEADER:       NS_FALL_THROUGH; /* fall through */
                    case SOCK_TOOMANYHEADERS:  NS_FALL_THROUGH; /* fall through */
                    case SOCK_CLOSE:
                        SockRelease(sockPtr, s, errno);
                        break;

                        /*
                         * Exceptions
                         */
                    case SOCK_READERROR:    NS_FALL_THROUGH; /* fall through */
                    case SOCK_CLOSETIMEOUT: NS_FALL_THROUGH; /* fall through */
                    case SOCK_ERROR:        NS_FALL_THROUGH; /* fall through */
                    case SOCK_READTIMEOUT:  NS_FALL_THROUGH; /* fall through */
                    case SOCK_SHUTERROR:    NS_FALL_THROUGH; /* fall through */
                    case SOCK_WRITEERROR:   NS_FALL_THROUGH; /* fall through */
                    case SOCK_WRITETIMEOUT:
                        drvPtr->stats.errors++;
                        Ns_Log(Warning,
                               "sockread returned unexpected result %s (err %s); close socket (%d)",
                               GetSockStateName(s),
                               ((errno != 0) ? strerror(errno) : NS_EMPTY_STRING),
                               sockPtr->sock);
                        SockRelease(sockPtr, s, errno);
                        break;
                    }
                } else {
                    /*
                     * Potentially blocking driver, NS_DRIVER_ASYNC is not defined
                     */
                    if (Ns_DiffTime(&sockPtr->timeout, &now, &diff) <= 0) {
                        drvPtr->stats.errors++;
                        Ns_Log(Notice, "read-ahead has some data, no async sock read ===== diff time %ld",
                               Ns_DiffTime(&sockPtr->timeout, &now, &diff));
                        sockPtr->keep = NS_FALSE;
                        SockRelease(sockPtr, SOCK_READTIMEOUT, 0);
                    } else {
                        if (SockQueue(sockPtr, &now) == NS_TIMEOUT) {
                            Push(sockPtr, waitPtr);
                        }
                    }
                }
            }
            sockPtr = nextPtr;
        }

        /*
         * Attempt to queue any pending connection after reversing the
         * list to ensure oldest connections are tried first.
         */
        if (waitPtr != NULL) {
            sockPtr = NULL;
            while ((nextPtr = waitPtr) != NULL) {
                waitPtr = nextPtr->nextPtr;
                Push(nextPtr, sockPtr);
            }

            while (sockPtr != NULL) {
                nextPtr = sockPtr->nextPtr;
                if (SockQueue(sockPtr, &now) == NS_TIMEOUT) {
                    Push(sockPtr, waitPtr);
                }
                sockPtr = nextPtr;
            }
        }

        /*
         * If no connections are waiting, attempt to accept more.
         */

        if (waitPtr == NULL) {
            /*
             * If configured, try to accept more than one request, under heavy load
             * this helps to process more requests
             */
            SockState s;
            bool      acceptMore = NS_TRUE;

            accepted = 0;
            while (acceptMore &&
                   accepted < drvPtr->acceptsize
                   && drvPtr->queuesize < drvPtr->maxqueuesize ) {
                bool gotRequests = NS_FALSE;

                /*
                 * Check for input data on all bind addresses. Stop checking,
                 * when one round of checking on all addresses fails.
                 */
                for (n = 0; n < nrBindaddrs; n++) {

                    if (
                        PollIn(&pdata, drvPtr->pidx[n])
                        && (s = SockAccept(drvPtr, pdata.pfds[drvPtr->pidx[n]].fd, &sockPtr, &now)) != SOCK_ERROR) {

                        switch (s) {
                        case SOCK_SPOOL:
                            drvPtr->stats.spooled++;
                            if (SockSpoolerQueue(drvPtr, sockPtr) == 0) {
                                Push(sockPtr, readPtr);
                            }
                            break;

                        case SOCK_MORE:
                            drvPtr->stats.partial++;
                            SockTimeout(sockPtr, &now, &drvPtr->recvwait);
                            Push(sockPtr, readPtr);
                            break;

                        case SOCK_READY:
                            if (SockQueue(sockPtr, &now) == NS_TIMEOUT) {
                                Push(sockPtr, waitPtr);
                            }
                            break;

                        case SOCK_BADHEADER:      NS_FALL_THROUGH; /* fall through */
                        case SOCK_BADREQUEST:     NS_FALL_THROUGH; /* fall through */
                        case SOCK_CLOSE:          NS_FALL_THROUGH; /* fall through */
                        case SOCK_CLOSETIMEOUT:   NS_FALL_THROUGH; /* fall through */
                        case SOCK_ENTITYTOOLARGE: NS_FALL_THROUGH; /* fall through */
                        case SOCK_ERROR:          NS_FALL_THROUGH; /* fall through */
                        case SOCK_READERROR:      NS_FALL_THROUGH; /* fall through */
                        case SOCK_READTIMEOUT:    NS_FALL_THROUGH; /* fall through */
                        case SOCK_SHUTERROR:      NS_FALL_THROUGH; /* fall through */
                        case SOCK_TOOMANYHEADERS: NS_FALL_THROUGH; /* fall through */
                        case SOCK_WRITEERROR:     NS_FALL_THROUGH; /* fall through */
                        case SOCK_WRITETIMEOUT:
                            Ns_Fatal("driver: SockAccept returned: %s", GetSockStateName(s));
                        }
                        accepted++;
                        gotRequests = NS_TRUE;
#ifdef __APPLE__
                        /*
                         * On Darwin, the first accept() succeeds typically, but it is
                         * useless to try, since this leads always to an EAGAIN
                         */
                        acceptMore = NS_FALSE;
                        break;
#endif
                    }
                }
                if (!gotRequests) {
                    acceptMore = NS_FALSE;
                }
            }
            if (accepted > 1) {
                Ns_Log(Notice, "... sockAccept accepted %d connections", accepted);
            }
        }

        /*
         * Check for shut down and get the list of any closing or
         * keep-alive sockets.
         */

        Ns_MutexLock(&drvPtr->lock);
        sockPtr          = drvPtr->closePtr;
        drvPtr->closePtr = NULL;
        flags            = drvPtr->flags;
        Ns_MutexUnlock(&drvPtr->lock);

        stopping = ((flags & DRIVER_SHUTDOWN) != 0u);

        /*
         * Update the timeout for each closing socket and add to the
         * close list if some data has been read from the socket
         * (i.e., it's not a closing keep-alive connection).
         */
        while (sockPtr != NULL) {
            nextPtr = sockPtr->nextPtr;
            if (sockPtr->keep) {

                assert(drvPtr == sockPtr->drvPtr);

                Ns_Log(DriverDebug, "setting keepwait %ld.%6ld for socket %d",
                       drvPtr->keepwait.sec, drvPtr->keepwait.usec,
                       sockPtr->sock);

                SockTimeout(sockPtr, &now, &drvPtr->keepwait);
                Push(sockPtr, readPtr);
            } else {

                /*
                 * Purely packet oriented drivers set on close the fd to
                 * NS_INVALID_SOCKET. Since we cannot "shutdown" an UDP-socket
                 * for writing, we bypass this call.
                 */
                assert(drvPtr == sockPtr->drvPtr);

                if (sockPtr->sock == NS_INVALID_SOCKET) {
                    SockRelease(sockPtr, SOCK_CLOSE, errno);

                    Ns_Log(DriverDebug, "DRIVER SockRelease: errno %d drvPtr->closewait %ld.%6ld",
                           errno, drvPtr->closewait.sec, drvPtr->closewait.usec);

                } else if (shutdown(sockPtr->sock, SHUT_WR) != 0) {
                    SockRelease(sockPtr, SOCK_SHUTERROR, errno);

                } else {
                    Ns_Log(DriverDebug, "setting closewait %ld.%6ld for socket %d",
                           drvPtr->closewait.sec,  drvPtr->closewait.usec, sockPtr->sock);
                    SockTimeout(sockPtr, &now, &drvPtr->closewait);
                    Push(sockPtr, closePtr);
                }
            }
            sockPtr = nextPtr;
        }

        /*
         * Close the active drivers if shutdown is pending.
         */

        if (stopping) {
            for (n = 0; n < nrBindaddrs; n++) {
                ns_sockclose(drvPtr->listenfd[n]);
                drvPtr->listenfd[n] = NS_INVALID_SOCKET;
            }
        }
    }

    PollFree(&pdata);

    Ns_Log(Notice, "exiting");
    Ns_MutexLock(&drvPtr->lock);
    drvPtr->flags |= DRIVER_STOPPED;
    Ns_CondBroadcast(&drvPtr->cond);
    Ns_MutexUnlock(&drvPtr->lock);

}