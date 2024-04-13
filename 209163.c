WriterThread(void *arg)
{
    SpoolerQueue   *queuePtr = (SpoolerQueue*)arg;
    int             err, pollTimeout;
    bool            stopping;
    Ns_Time         now;
    Sock           *sockPtr;
    const Driver   *drvPtr;
    WriterSock     *curPtr, *nextPtr, *writePtr;
    PollData        pdata;
    Tcl_HashTable   pools;     /* used for accumulating bandwidth per pool */

    Ns_ThreadSetName("-writer%d-", queuePtr->id);
    queuePtr->threadName = Ns_ThreadGetName();

    Tcl_InitHashTable(&pools, TCL_ONE_WORD_KEYS);

    /*
     * Loop forever until signaled to shut down and all
     * connections are complete and gracefully closed.
     */

    Ns_Log(Notice, "writer%d: accepting connections", queuePtr->id);

    PollCreate(&pdata);
    writePtr = NULL;
    stopping = NS_FALSE;

    while (!stopping) {
        char charBuffer[1];

        /*
         * If there are any write sockets, set the bits.
         */

        PollReset(&pdata);
        (void)PollSet(&pdata, queuePtr->pipe[0], (short)POLLIN, NULL);

        if (writePtr == NULL) {
            pollTimeout = 30 * 1000;
        } else {

            /*
             * If per-pool bandwidth management is requested, compute the base
             * data for the adjustment. If there is no bandwidth management
             * requested, there is no slowdow.
             */
            if (NsWriterBandwidthManagement) {
                WriterPerPoolRates(writePtr, &pools);
            }

            /*
             * There are writers active. Determine on which writers we poll
             * and compute the maximal poll wait time.
             */
            pollTimeout = 1000;
            for (curPtr = writePtr; curPtr != NULL; curPtr = curPtr->nextPtr) {
                int sleepTimeMs = 0;

                Ns_Log(DriverDebug, "### Writer poll collect %p size %" PRIdz
                       " streaming %d rateLimit %d",
                       (void *)curPtr, curPtr->size, curPtr->doStream, curPtr->rateLimit);

                if (curPtr->rateLimit > 0
                    && curPtr->nsent > 0
                    && curPtr->currentRate > 0
                    ) {
                    int  currentMs, targetTimeMs;

                    /*
                     * Perform per-pool rate management, when
                     *  - a poolLimit is provided,
                     *  - we have performance data of thee pool, and
                     *  - changes are possible (as flagged by deltaPercentage).
                     */
                    if (NsWriterBandwidthManagement
                        && curPtr->poolPtr->rate.poolLimit > 0
                        && curPtr->infoPtr != NULL
                        && curPtr->infoPtr->deltaPercentage != 0
                        ) {
                        /*
                         * Only adjust data for busy writer jobs, which
                         * are close to their limits.
                         */
                        bool onLimit = (curPtr->currentRate*100 / curPtr->rateLimit) > 90;

                        Ns_Log(DriverDebug, "we allowed %d we use %d on limit %d (%d) , we can do %d%%",
                               curPtr->rateLimit,  curPtr->currentRate,
                               (int)onLimit, curPtr->currentRate*100/curPtr->rateLimit,
                               curPtr->infoPtr->deltaPercentage);
                        if (onLimit) {
                            /*
                             * Compute new rate limit based on
                             * positive/negative delta percentage.
                             */
                            int newRate = curPtr->currentRate +
                                (curPtr->currentRate * curPtr->infoPtr->deltaPercentage / 100);
                            /*
                             * Sanity checks:
                             *  - never allow more than poolLimit
                             *  - never kill connections completely (e.g. minRate 5KB/s)
                             */
                            if (newRate > curPtr->poolPtr->rate.poolLimit) {
                                newRate = curPtr->poolPtr->rate.poolLimit;
                            } else if (newRate < 5) {
                                newRate = 5;
                            }
                            Ns_Log(Notice, "... pool '%s' new rate limit changed from %d to %d KB/s (delta %d%%)",
                                   curPtr->poolPtr->pool, curPtr->rateLimit, newRate,
                                   curPtr->infoPtr->deltaPercentage);
                            curPtr->rateLimit = newRate;
                        }
                    }

                    /*
                     * Adjust rate to the rate limit.
                     */
                    currentMs    = (int)(curPtr->nsent/(Tcl_WideInt)curPtr->currentRate);
                    targetTimeMs = (int)(curPtr->nsent/(Tcl_WideInt)curPtr->rateLimit);
                    sleepTimeMs = 1 + targetTimeMs - currentMs;
                    Ns_Log(WriterDebug, "### Writer(%d)"
                           " byte sent %" TCL_LL_MODIFIER "d msecs %d rate %d KB/s"
                           " targetRate %d KB/s sleep %d",
                           curPtr->sockPtr->sock,
                           curPtr->nsent, currentMs,
                           curPtr->currentRate,
                           curPtr->rateLimit,
                           sleepTimeMs);
                }

                if (likely(curPtr->size > 0u)) {
                    if (sleepTimeMs <= 0) {
                        SockPoll(curPtr->sockPtr, (short)POLLOUT, &pdata);
                        pollTimeout = -1;
                    } else {
                        pollTimeout = MIN(sleepTimeMs, pollTimeout);
                    }
                } else if (unlikely(curPtr->doStream == NS_WRITER_STREAM_FINISH)) {
                    pollTimeout = -1;
                }
            }
        }
        Ns_Log(DriverDebug, "### Writer final pollTimeout %d", pollTimeout);

        /*
         * Select and drain the trigger pipe if necessary.
         */
        (void) PollWait(&pdata, pollTimeout);

        if (PollIn(&pdata, 0) && unlikely(ns_recv(queuePtr->pipe[0], charBuffer, 1u, 0) != 1)) {
            Ns_Fatal("writer: trigger ns_recv() failed: %s",
                     ns_sockstrerror(ns_sockerrno));
        }

        /*
         * Write to all available sockets
         */
        Ns_GetTime(&now);
        curPtr = writePtr;
        writePtr = NULL;

        while (curPtr != NULL) {
            NsWriterStreamState doStream;
            SpoolerState        spoolerState = SPOOLER_OK;

            nextPtr = curPtr->nextPtr;
            sockPtr = curPtr->sockPtr;
            err = 0;

            /*
             * The truth value of doStream does not change through
             * concurrency.
             */
            doStream = curPtr->doStream;

            if (unlikely(PollHup(&pdata, sockPtr->pidx))) {
                Ns_Log(DriverDebug, "### Writer %p reached POLLHUP fd %d", (void *)curPtr, sockPtr->sock);
                spoolerState = SPOOLER_CLOSE;
                err = 0;
                curPtr->infoPtr = WriterGetInfoPtr(curPtr, &pools);
                curPtr->infoPtr->currentPoolRate += curPtr->currentRate;


            } else if (likely(PollOut(&pdata, sockPtr->pidx)) || (doStream == NS_WRITER_STREAM_FINISH)) {
                /*
                 * The socket is writable, we can compute the rate, when
                 * something was sent already and some kind of rate limiting
                 * is in place ... and we have sent enough data to make a good
                 * estimate (just after the 2nd send, so more than driver
                 * buffer size.
                 */
                Ns_Log(DriverDebug, "Socket of pool '%s' is writable, writer limit %d nsent %ld",
                       curPtr->poolPtr->pool, curPtr->rateLimit, (long)curPtr->nsent);

                if (curPtr->rateLimit > 0
                    && (size_t)curPtr->nsent > curPtr->sockPtr->drvPtr->bufsize
                    )  {
                    Ns_Time diff;
                    long    currentMs;

                    Ns_DiffTime(&now, &curPtr->startTime, &diff);
                    currentMs = Ns_TimeToMilliseconds(&diff);
                    if (currentMs > 0) {
                        curPtr->currentRate = (int)((curPtr->nsent)/(Tcl_WideInt)currentMs);
                        Ns_Log(DriverDebug,
                               "Socket of pool '%s' is writable, currentMs %ld has updated current rate %d",
                               curPtr->poolPtr->pool, currentMs,curPtr->currentRate);
                    }
                }
                Ns_Log(DriverDebug,
                       "### Writer %p can write to client fd %d (trigger %d) streaming %.6x"
                       " size %" PRIdz " nsent %" TCL_LL_MODIFIER "d bufsize %" PRIdz,
                       (void *)curPtr, sockPtr->sock, PollIn(&pdata, 0), doStream,
                       curPtr->size, curPtr->nsent, curPtr->c.file.bufsize);
                if (unlikely(curPtr->size < 1u)) {
                    /*
                     * Size < 1 means that everything was sent.
                     */
                    if (doStream != NS_WRITER_STREAM_ACTIVE) {
                        if (doStream == NS_WRITER_STREAM_FINISH) {
                            Ns_ReleaseTemp(curPtr->fd);
                        }
                        spoolerState = SPOOLER_CLOSE;
                    }
                } else {
                    /*
                     * If size > 0, there is still something to send.
                     * If we are spooling from a file, read some data
                     * from the (spool) file and place it into curPtr->c.file.buf.
                     */
                    if (curPtr->fd != NS_INVALID_FD) {
                        spoolerState = WriterReadFromSpool(curPtr);
                    }

                    if (spoolerState == SPOOLER_OK) {
                        spoolerState = WriterSend(curPtr, &err);
                    }
                }
            } else {

                /*
                 *  Mark when first timeout occurred or check if it is already
                 *  for too long and we need to stop this socket
                 */
                if (sockPtr->timeout.sec == 0) {
                    Ns_Log(DriverDebug, "Writer %p fd %d setting sendwait %ld.%6ld",
                           (void *)curPtr, sockPtr->sock,
                           curPtr->sockPtr->drvPtr->sendwait.sec,
                           curPtr->sockPtr->drvPtr->sendwait.usec);
                    SockTimeout(sockPtr, &now, &curPtr->sockPtr->drvPtr->sendwait);
                } else if (Ns_DiffTime(&sockPtr->timeout, &now, NULL) <= 0) {
                    Ns_Log(DriverDebug, "Writer %p fd %d timeout", (void *)curPtr, sockPtr->sock);
                    err          = ETIMEDOUT;
                    spoolerState = SPOOLER_CLOSETIMEOUT;
                }
            }

            /*
             * Check result status and close the socket in case of
             * timeout or completion
             */

            Ns_MutexLock(&queuePtr->lock);
            if (spoolerState == SPOOLER_OK) {
                if (curPtr->size > 0u || doStream == NS_WRITER_STREAM_ACTIVE) {
                    Ns_Log(DriverDebug,
                           "Writer %p continue OK (size %" PRIdz ") => PUSH",
                           (void *)curPtr, curPtr->size);
                    Push(curPtr, writePtr);
                } else {
                    Ns_Log(DriverDebug,
                           "Writer %p done OK (size %" PRIdz ") => RELEASE",
                           (void *)curPtr, curPtr->size);
                    WriterSockRelease(curPtr);
                }
            } else {
                /*
                 * spoolerState might be SPOOLER_CLOSE or SPOOLER_*TIMEOUT, or SPOOLER_*ERROR
                 */
                Ns_Log(DriverDebug,
                       "Writer %p fd %d release, not OK (status %d) => RELEASE",
                       (void *)curPtr, curPtr->sockPtr->sock, (int)spoolerState);
                curPtr->status = spoolerState;
                curPtr->err    = err;
                WriterSockRelease(curPtr);
            }
            Ns_MutexUnlock(&queuePtr->lock);
            curPtr = nextPtr;
        }

        /*
         * Add more sockets to the writer queue
         */

        if (queuePtr->sockPtr != NULL) {
            Ns_MutexLock(&queuePtr->lock);
            if (queuePtr->sockPtr != NULL) {
                curPtr = queuePtr->sockPtr;
                queuePtr->sockPtr = NULL;
                while (curPtr != NULL) {
                    nextPtr = curPtr->nextPtr;
                    sockPtr = curPtr->sockPtr;
                    drvPtr  = sockPtr->drvPtr;
                    SockTimeout(sockPtr, &now, &drvPtr->sendwait);
                    Push(curPtr, writePtr);
                    queuePtr->queuesize++;
                    curPtr = nextPtr;
                }
                queuePtr->curPtr = writePtr;
            }
            Ns_MutexUnlock(&queuePtr->lock);
        }

        /*
         * Check for shutdown
         */

        stopping = queuePtr->shutdown;
    }
    PollFree(&pdata);

    {
        /*
         * Free ConnPoolInfo
         */
        Tcl_HashSearch  search;
        Tcl_HashEntry  *hPtr = Tcl_FirstHashEntry(&pools, &search);
        while (hPtr != NULL) {
            ConnPoolInfo *infoPtr = (ConnPoolInfo *)Tcl_GetHashValue(hPtr);
            ns_free(infoPtr);
            hPtr = Tcl_NextHashEntry(&search);
        }
        /*
         * Delete the hash table for pools.
         */
        Tcl_DeleteHashTable(&pools);
    }


    Ns_Log(Notice, "exiting");

    Ns_MutexLock(&queuePtr->lock);
    queuePtr->stopped = NS_TRUE;
    Ns_CondBroadcast(&queuePtr->cond);
    Ns_MutexUnlock(&queuePtr->lock);
}