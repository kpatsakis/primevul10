SpoolerThread(void *arg)
{
    SpoolerQueue  *queuePtr = (SpoolerQueue*)arg;
    char           charBuffer[1];
    int            pollTimeout;
    bool           stopping;
    Sock          *sockPtr, *nextPtr, *waitPtr, *readPtr;
    Ns_Time        now, diff;
    const Driver  *drvPtr;
    PollData       pdata;

    Ns_ThreadSetName("-spooler%d-", queuePtr->id);
    queuePtr->threadName = Ns_ThreadGetName();

    /*
     * Loop forever until signaled to shut down and all
     * connections are complete and gracefully closed.
     */

    Ns_Log(Notice, "spooler%d: accepting connections", queuePtr->id);

    PollCreate(&pdata);
    Ns_GetTime(&now);
    waitPtr = readPtr = NULL;
    stopping = NS_FALSE;

    while (!stopping) {

        /*
         * If there are any read sockets, set the bits
         * and determine the minimum relative timeout.
         */

        PollReset(&pdata);
        (void)PollSet(&pdata, queuePtr->pipe[0], (short)POLLIN, NULL);

        if (readPtr == NULL) {
            pollTimeout = 30 * 1000;
        } else {
            sockPtr = readPtr;
            while (sockPtr != NULL) {
                SockPoll(sockPtr, (short)POLLIN, &pdata);
                sockPtr = sockPtr->nextPtr;
            }
            pollTimeout = -1;
        }

        /*
         * Select and drain the trigger pipe if necessary.
         */

        /*n =*/ (void) PollWait(&pdata, pollTimeout);

        if (PollIn(&pdata, 0) && unlikely(ns_recv(queuePtr->pipe[0], charBuffer, 1u, 0) != 1)) {
            Ns_Fatal("spooler: trigger ns_recv() failed: %s",
                     ns_sockstrerror(ns_sockerrno));
        }

        /*
         * Attempt read-ahead of any new connections.
         */

        Ns_GetTime(&now);
        sockPtr = readPtr;
        readPtr = NULL;

        while (sockPtr != NULL) {
            nextPtr = sockPtr->nextPtr;
            drvPtr  = sockPtr->drvPtr;
            if (unlikely(PollHup(&pdata, sockPtr->pidx))) {
                /*
                 * Peer has closed the connection
                 */
                SockRelease(sockPtr, SOCK_CLOSE, 0);

            } else if (!PollIn(&pdata, sockPtr->pidx)) {
                /*
                 * Got no data
                 */
                if (Ns_DiffTime(&sockPtr->timeout, &now, &diff) <= 0) {
                    SockRelease(sockPtr, SOCK_READTIMEOUT, 0);
                    queuePtr->queuesize--;
                } else {
                    Push(sockPtr, readPtr);
                }
            } else {
                /*
                 * Got some data
                 */
                SockState n = SockRead(sockPtr, 1, &now);
                switch (n) {
                case SOCK_MORE:
                    SockTimeout(sockPtr, &now, &drvPtr->recvwait);
                    Push(sockPtr, readPtr);
                    break;

                case SOCK_READY:
                    assert(sockPtr->reqPtr != NULL);
                    SockSetServer(sockPtr);
                    Push(sockPtr, waitPtr);
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
                case SOCK_SPOOL:          NS_FALL_THROUGH; /* fall through */
                case SOCK_TOOMANYHEADERS: NS_FALL_THROUGH; /* fall through */
                case SOCK_WRITEERROR:     NS_FALL_THROUGH; /* fall through */
                case SOCK_WRITETIMEOUT:
                    SockRelease(sockPtr, n, errno);
                    queuePtr->queuesize--;
                    break;
                }
            }
            sockPtr = nextPtr;
        }

        /*
         * Attempt to queue any pending connection
         * after reversing the list to ensure oldest
         * connections are tried first.
         */

        if (waitPtr != NULL) {
            sockPtr = NULL;
            while ((nextPtr = waitPtr) != NULL) {
                waitPtr = nextPtr->nextPtr;
                Push(nextPtr, sockPtr);
            }
            while (sockPtr != NULL) {
                nextPtr = sockPtr->nextPtr;
                if (!NsQueueConn(sockPtr, &now)) {
                    Push(sockPtr, waitPtr);
                } else {
                    queuePtr->queuesize--;
                }
                sockPtr = nextPtr;
            }
        }

        /*
         * Add more connections from the spooler queue
         */

        Ns_MutexLock(&queuePtr->lock);
        if (waitPtr == NULL) {
            sockPtr = (Sock*)queuePtr->sockPtr;
            queuePtr->sockPtr = NULL;
            while (sockPtr != NULL) {
                nextPtr = sockPtr->nextPtr;
                drvPtr  = sockPtr->drvPtr;
                SockTimeout(sockPtr, &now, &drvPtr->recvwait);
                Push(sockPtr, readPtr);
                queuePtr->queuesize++;
                sockPtr = nextPtr;
            }
        }

        /*
         * Check for shutdown
         */

        stopping = queuePtr->shutdown;
        Ns_MutexUnlock(&queuePtr->lock);
    }
    PollFree(&pdata);

    Ns_Log(Notice, "exiting");

    Ns_MutexLock(&queuePtr->lock);
    queuePtr->stopped = NS_TRUE;
    Ns_CondBroadcast(&queuePtr->cond);
    Ns_MutexUnlock(&queuePtr->lock);
}