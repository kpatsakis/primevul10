AsyncWriterThread(void *arg)
{
    SpoolerQueue   *queuePtr = (SpoolerQueue*)arg;
    char            charBuffer[1];
    int             pollTimeout;
    Ns_ReturnCode   status;
    bool            stopping;
    AsyncWriteData *curPtr, *nextPtr, *writePtr;
    PollData        pdata;

    Ns_ThreadSetName("-asynclogwriter%d-", queuePtr->id);
    queuePtr->threadName = Ns_ThreadGetName();

    /*
     * Allocate and initialize controlling variables
     */

    PollCreate(&pdata);
    writePtr = NULL;
    stopping = NS_FALSE;

    /*
     * Loop forever until signaled to shutdown and all
     * connections are complete and gracefully closed.
     */

    while (!stopping) {

        /*
         * Always listen to the trigger pipe. We could as well perform
         * in the writer thread async write operations, but for the
         * effect of reducing latency in connection threads, this is
         * not an issue. To keep things simple, we perform the
         * typically small write operations without testing for POLLOUT.
         */

        PollReset(&pdata);
        (void)PollSet(&pdata, queuePtr->pipe[0], (short)POLLIN, NULL);

        if (writePtr == NULL) {
            pollTimeout = 30 * 1000;
        } else {
            pollTimeout = 0;
        }

        /*
         * Wait for data
         */
        /*n =*/ (void) PollWait(&pdata, pollTimeout);

        /*
         * Select and drain the trigger pipe if necessary.
         */
        if (PollIn(&pdata, 0)) {
            if (ns_recv(queuePtr->pipe[0], charBuffer, 1u, 0) != 1) {
                Ns_Fatal("asynclogwriter: trigger ns_recv() failed: %s",
                         ns_sockstrerror(ns_sockerrno));
            }
            if (queuePtr->stopped) {
                /*
                 * Drain the queue from everything
                 */
                for (curPtr = writePtr; curPtr != NULL;  curPtr = curPtr->nextPtr) {
                    ssize_t written = ns_write(curPtr->fd, curPtr->buf, curPtr->bufsize);
                    if (unlikely(written != (ssize_t)curPtr->bufsize)) {
                        WriteWarningRaw("drain writer", curPtr->fd, curPtr->bufsize, written);
                    }
                }
                writePtr = NULL;

                for (curPtr = queuePtr->sockPtr; curPtr != NULL;  curPtr = curPtr->nextPtr) {
                    ssize_t written = ns_write(curPtr->fd, curPtr->buf, curPtr->bufsize);
                    if (unlikely(written != (ssize_t)curPtr->bufsize)) {
                        WriteWarningRaw("drain queue", curPtr->fd, curPtr->bufsize, written);
                    }
                }
                queuePtr->sockPtr = NULL;

                /*
                 * Notify the caller (normally
                 * NsAsyncWriterQueueDisable()) that we are done
                 */
                Ns_CondBroadcast(&queuePtr->cond);
            }
        }

        /*
         * Write to all available file descriptors
         */

        curPtr = writePtr;
        writePtr = NULL;

        while (curPtr != NULL) {
            ssize_t written;

            nextPtr = curPtr->nextPtr;
            status = NS_OK;

            /*
             * Write the actual data and allow for partial write operations.
             */
            written = ns_write(curPtr->fd, curPtr->buf, curPtr->bufsize);
            if (unlikely(written < 0)) {
                status = NS_ERROR;
            } else {
                curPtr->size -= (size_t)written;
                curPtr->nsent += written;
                curPtr->bufsize -= (size_t)written;
                if (curPtr->data != NULL) {
                    curPtr->buf += written;
                }
            }

            if (unlikely(status != NS_OK)) {
                AsyncWriterRelease(curPtr);
                queuePtr->queuesize--;
            } else {

                /*
                 * The write operation was successful. Check if there
                 * is some remaining data to write. If not we are done
                 * with this request can release the write buffer.
                 */
                if (curPtr->size > 0u) {
                    Push(curPtr, writePtr);
                } else {
                    AsyncWriterRelease(curPtr);
                    queuePtr->queuesize--;
                }
            }

            curPtr = nextPtr;
        }


        /*
         * Check for shutdown
         */
        stopping = queuePtr->shutdown;
        if (stopping) {
            curPtr = queuePtr->sockPtr;
            assert(writePtr == NULL);
            while (curPtr != NULL) {
                ssize_t written = ns_write(curPtr->fd, curPtr->buf, curPtr->bufsize);
                if (unlikely(written != (ssize_t)curPtr->bufsize)) {
                    WriteWarningRaw("shutdown", curPtr->fd, curPtr->bufsize, written);
                }
                curPtr = curPtr->nextPtr;
            }
        } else {
            /*
             * Add fresh jobs to the writer queue. This means actually to
             * move jobs from queuePtr->sockPtr (kept name for being able
             * to use the same queue as above) to the currently active
             * jobs in queuePtr->curPtr.
             */
            Ns_MutexLock(&queuePtr->lock);
            curPtr = queuePtr->sockPtr;
            queuePtr->sockPtr = NULL;
            while (curPtr != NULL) {
                nextPtr = curPtr->nextPtr;
                Push(curPtr, writePtr);
                queuePtr->queuesize++;
                curPtr = nextPtr;
            }
            queuePtr->curPtr = writePtr;
            Ns_MutexUnlock(&queuePtr->lock);
        }

    }

    PollFree(&pdata);

    queuePtr->stopped = NS_TRUE;
    Ns_Log(Notice, "exiting");

}