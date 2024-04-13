WriterSetupStreamingMode(Conn *connPtr, struct iovec *bufs, int nbufs, int *fdPtr)
{
    bool           first;
    size_t         wrote = 0u;
    WriterSock    *wrSockPtr1;
    Ns_ReturnCode  status = NS_OK;

    NS_NONNULL_ASSERT(connPtr != NULL);
    NS_NONNULL_ASSERT(bufs != NULL);
    NS_NONNULL_ASSERT(fdPtr != NULL);

    Ns_Log(DriverDebug, "NsWriterQueue: streaming writer job");

    if (connPtr->fd == 0) {
        /*
         * Create a new temporary spool file and provide the fd to the
         * connection thread via connPtr.
         */
        first = NS_TRUE;
        wrSockPtr1 = NULL;

        *fdPtr = Ns_GetTemp();
        connPtr->fd = *fdPtr;

        Ns_Log(DriverDebug, "NsWriterQueue: new tmp file has fd %d", *fdPtr);

    } else {
        /*
         * Reuse previously created spool file.
         */
        first = NS_FALSE;
        wrSockPtr1 = WriterSockRequire(connPtr);

        if (wrSockPtr1 == NULL) {
            Ns_Log(Notice,
                   "NsWriterQueue: writer job was already canceled (fd %d); maybe user dropped connection",
                   connPtr->fd);
            return NS_ERROR;

        } else {
            /*
             * lock only, when first == NS_FALSE.
             */
            Ns_MutexLock(&wrSockPtr1->c.file.fdlock);
            (void)ns_lseek(connPtr->fd, 0, SEEK_END);
        }
    }

    /*
     * For the time being, handle just "string data" in streaming
     * output (iovec bufs). Write the content to the spool file.
     */
    {
        int i;

        for (i = 0; i < nbufs; i++) {
            ssize_t j = ns_write(connPtr->fd, bufs[i].iov_base, bufs[i].iov_len);

            if (j > 0) {
                wrote += (size_t)j;
                Ns_Log(Debug, "NsWriterQueue: fd %d [%d] spooled %" PRIdz " of %" PRIiovlen " OK %d",
                       connPtr->fd, i, j, bufs[i].iov_len, (j == (ssize_t)bufs[i].iov_len));
            } else {
                Ns_Log(Warning, "NsWriterQueue: spool to fd %d write operation failed",
                       connPtr->fd);
            }
        }
    }

    if (first) {
        //bufs = NULL;
        connPtr->nContentSent = wrote;
#ifndef _WIN32
        /*
         * sock_set_blocking can't be used under windows, since sockets
         * are under windows no file descriptors.
         */
        (void)ns_sock_set_blocking(connPtr->fd, NS_FALSE);
#endif
        /*
         * Fall through to register stream writer with temp file
         */
    } else {
        WriterSock *writerSockPtr;

        /*
         * This is a later streaming operation, where the writer job
         * (strWriter) was previously established.
         */
        assert(wrSockPtr1 != NULL);
        /*
         * Update the controlling variables (size and toread) in the connPtr,
         * and the length info for the access log, and trigger the writer to
         * notify it about the change.
         */

        writerSockPtr = (WriterSock *)connPtr->strWriter;
        writerSockPtr->size += wrote;
        writerSockPtr->c.file.toRead += wrote;
        Ns_MutexUnlock(&wrSockPtr1->c.file.fdlock);

        connPtr->nContentSent += wrote;
        if (likely(wrSockPtr1->queuePtr != NULL)) {
            SockTrigger(wrSockPtr1->queuePtr->pipe[1]);
        }
        WriterSockRelease(wrSockPtr1);
        status = NS_FILTER_BREAK;
    }

    return status;
}