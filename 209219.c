WriterSend(WriterSock *curPtr, int *err) {
    const struct iovec *bufs;
    struct iovec        vbuf;
    int                 nbufs;
    SpoolerState        status = SPOOLER_OK;
    size_t              toWrite;
    ssize_t             n;

    NS_NONNULL_ASSERT(curPtr != NULL);
    NS_NONNULL_ASSERT(err != NULL);

    /*
     * Prepare send operation
     */
    if (curPtr->fd != NS_INVALID_FD) {
        /*
         * We have a valid file descriptor, send data from file.
         *
         * Prepare sending a single buffer with curPtr->c.file.bufsize bytes
         * from the curPtr->c.file.buf to the client.
         */
        vbuf.iov_len = curPtr->c.file.bufsize;
        vbuf.iov_base = (void *)curPtr->c.file.buf;
        bufs = &vbuf;
        nbufs = 1;
        toWrite = curPtr->c.file.bufsize;
    } else {
        int i;

        /*
         * Prepare sending multiple memory buffers.  Get length of remaining
         * buffers.
         */
        toWrite = 0u;
        for (i = 0; i < curPtr->c.mem.nsbufs; i ++) {
            toWrite += curPtr->c.mem.sbufs[i].iov_len;
        }
        Ns_Log(DriverDebug,
               "### Writer wants to send remainder nbufs %d len %" PRIdz,
               curPtr->c.mem.nsbufs, toWrite);

        /*
         * Add buffers from the source and fill structure up to max
         */
        while (curPtr->c.mem.bufIdx  < curPtr->c.mem.nbufs &&
               curPtr->c.mem.sbufIdx < UIO_SMALLIOV) {
            const struct iovec *vPtr = &curPtr->c.mem.bufs[curPtr->c.mem.bufIdx];

            if (vPtr->iov_len > 0u && vPtr->iov_base != NULL) {

                Ns_Log(DriverDebug,
                       "### Writer copies source %d to scratch %d len %" PRIiovlen,
                       curPtr->c.mem.bufIdx, curPtr->c.mem.sbufIdx, vPtr->iov_len);

                toWrite += Ns_SetVec(curPtr->c.mem.sbufs, curPtr->c.mem.sbufIdx++,
                                     vPtr->iov_base, vPtr->iov_len);
                curPtr->c.mem.nsbufs++;
            }
            curPtr->c.mem.bufIdx++;
        }

        bufs  = curPtr->c.mem.sbufs;
        nbufs = curPtr->c.mem.nsbufs;
        Ns_Log(DriverDebug, "### Writer wants to send %d bufs size %" PRIdz,
               nbufs, toWrite);
    }

    /*
     * Perform the actual send operation.
     */
    n = NsDriverSend(curPtr->sockPtr, bufs, nbufs, 0u);

    if (n == -1) {
        *err = ns_sockerrno;
        status = SPOOLER_WRITEERROR;
    } else {
        /*
         * We have sent zero or more bytes.
         */
        if (curPtr->doStream != NS_WRITER_STREAM_NONE) {
            Ns_MutexLock(&curPtr->c.file.fdlock);
            curPtr->size -= (size_t)n;
            Ns_MutexUnlock(&curPtr->c.file.fdlock);
        } else {
            curPtr->size -= (size_t)n;
        }
        curPtr->nsent += n;
        curPtr->sockPtr->timeout.sec = 0;

        if (curPtr->fd != NS_INVALID_FD) {
            /*
             * File-descriptor based send operation. Reduce the (remainig)
             * buffer size the amount of data sent and adjust the buffer
             * offset. For partial send operations, this will lead to a
             * remaining buffer size > 0.
             */
            curPtr->c.file.bufsize -= (size_t)n;
            curPtr->c.file.bufoffset = (off_t)n;

        } else {
            if (n < (ssize_t)toWrite) {
                /*
                 * We have a partial transmit from the iovec
                 * structure. We have to compact it to fill content in
                 * the next round.
                 */
                curPtr->c.mem.sbufIdx = Ns_ResetVec(curPtr->c.mem.sbufs, curPtr->c.mem.nsbufs, (size_t)n);
                curPtr->c.mem.nsbufs -= curPtr->c.mem.sbufIdx;

                memmove(curPtr->c.mem.sbufs, curPtr->c.mem.sbufs + curPtr->c.mem.sbufIdx,
                        /* move the iovecs to the start of the scratch buffers */
                        sizeof(struct iovec) * (size_t)curPtr->c.mem.nsbufs);
            }
        }
    }

    return status;
}