NsWriterQueue(Ns_Conn *conn, size_t nsend,
              Tcl_Channel chan, FILE *fp, int fd,
              struct iovec *bufs, int nbufs,
              const Ns_FileVec *filebufs, int nfilebufs,
              bool everysize)
{
    Conn          *connPtr;
    WriterSock    *wrSockPtr;
    SpoolerQueue  *queuePtr;
    DrvWriter     *wrPtr;
    bool           trigger = NS_FALSE;
    size_t         headerSize;
    Ns_ReturnCode  status = NS_OK;
    Ns_FileVec    *fbufs = NULL;
    int            nfbufs = 0;

    NS_NONNULL_ASSERT(conn != NULL);
    connPtr = (Conn *)conn;

    if (unlikely(connPtr->sockPtr == NULL)) {
        Ns_Log(Warning,
               "NsWriterQueue: called without sockPtr size %" PRIdz " bufs %d flags %.6x stream %.6x chan %p fd %d",
               nsend, nbufs, connPtr->flags, connPtr->flags & NS_CONN_STREAM,
               (void *)chan, fd);
        status = NS_ERROR;
        wrPtr = NULL;
    } else {

        wrPtr = &connPtr->sockPtr->drvPtr->writer;

        Ns_Log(DriverDebug,
               "NsWriterQueue: size %" PRIdz " bufs %p (%d) flags %.6x stream %.6x chan %p fd %d thread %d",
               nsend, (void *)bufs, nbufs, connPtr->flags, connPtr->flags & NS_CONN_STREAM,
               (void *)chan, fd, wrPtr->threads);

        if (unlikely(wrPtr->threads == 0)) {
            Ns_Log(DriverDebug, "NsWriterQueue: no writer threads configured");
            status = NS_ERROR;

        } else if (nsend < (size_t)wrPtr->writersize && !everysize && connPtr->fd == 0) {
            Ns_Log(DriverDebug, "NsWriterQueue: file is too small(%" PRIdz " < %" PRIdz ")",
                   nsend, wrPtr->writersize);
            status = NS_ERROR;
        }
    }
    if (status != NS_OK) {
        return status;
    }

    assert(wrPtr != NULL);

    /*
     * In streaming mode, setup a temporary fd which is used as input and
     * output. Streaming i/o will append to the file, while the write will
     * read from it.
     */
    if (((connPtr->flags & NS_CONN_STREAM) != 0u) || connPtr->fd > 0) {

        if (wrPtr->doStream == NS_WRITER_STREAM_NONE) {
            status = NS_ERROR;
        } else if (unlikely(fp != NULL || fd != NS_INVALID_FD)) {
            Ns_Log(DriverDebug, "NsWriterQueue: does not stream from this source via writer");
            status = NS_ERROR;
        } else {
            status = WriterSetupStreamingMode(connPtr, bufs, nbufs, &fd);
        }

        if (unlikely(status != NS_OK)) {
            if (status == NS_FILTER_BREAK) {
                status = NS_OK;
            }
            return status;
        }

        /*
         * As a result of successful WriterSetupStreamingMode(), we have fd
         * set.
         */
        assert(fd != NS_INVALID_FD);

    } else {
        if (fp != NULL) {
            /*
             * The client provided an open file pointer and closes it
             */
            fd = ns_dup(fileno(fp));
        } else if (fd != NS_INVALID_FD) {
            /*
             * The client provided an open file descriptor and closes it
             */
            fd = ns_dup(fd);
        } else if (chan != NULL) {
            ClientData clientData;
            /*
             * The client provided an open Tcl channel and closes it
             */
            if (Tcl_GetChannelHandle(chan, TCL_READABLE, &clientData) != TCL_OK) {
                return NS_ERROR;
            }
            fd = ns_dup(PTR2INT(clientData));
        } else if (filebufs != NULL && nfilebufs > 0) {
            /*
             * The client provided Ns_FileVec with open files. The client is
             * responsible for closing it, like in all other cases.
             */
            size_t i;

            /*
             * This is the only case, where fbufs will be != NULL,
             * i.e. keeping a duplicate of the passed-in Ns_FileVec structure
             * for which the client is responsible.
             */
            fbufs = (Ns_FileVec *)ns_calloc((size_t)nfilebufs, sizeof(Ns_FileVec));
            nfbufs = nfilebufs;

            for (i = 0u; i < (size_t)nfilebufs; i++) {
                fbufs[i].fd = ns_dup(filebufs[i].fd);
                fbufs[i].length = filebufs[i].length;
                fbufs[i].offset = filebufs[i].offset;
            }
            /*
             * Place the fd of the first Ns_FileVec to fd.
             */
            fd = fbufs[0].fd;

            Ns_Log(DriverDebug, "NsWriterQueue: filevec mode, take first fd %d tosend %lu", fd, nsend);
        }
    }

    Ns_Log(DriverDebug, "NsWriterQueue: writer threads %d nsend %" PRIdz " writersize %" PRIdz,
           wrPtr->threads, nsend, wrPtr->writersize);

    assert(connPtr->poolPtr != NULL);
    connPtr->poolPtr->stats.spool++;

    wrSockPtr = (WriterSock *)ns_calloc(1u, sizeof(WriterSock));
    wrSockPtr->sockPtr = connPtr->sockPtr;
    wrSockPtr->poolPtr = connPtr->poolPtr;  /* just for being able to trace back the origin, e.g. list */
    wrSockPtr->sockPtr->timeout.sec = 0;
    wrSockPtr->flags = connPtr->flags;
    wrSockPtr->refCount = 1;
    /*
     * Take the rate limit from the connection.
     */
    wrSockPtr->rateLimit = connPtr->rateLimit;
    if (wrSockPtr->rateLimit == -1) {
        /*
         * The value was not specified via connection. Use either the pool
         * limit as a base for the computation or fall back to the driver
         * default value.
         */
        if (connPtr->poolPtr->rate.poolLimit > 0) {
            /*
             * Very optimistic start value, but values will float through via
             * bandwidth management.
             */
            wrSockPtr->rateLimit = connPtr->poolPtr->rate.poolLimit / 2;
        } else {
            wrSockPtr->rateLimit = wrPtr->rateLimit;
        }
    }
    Ns_Log(WriterDebug, "### Writer(%d): initial rate limit %d KB/s",
           wrSockPtr->sockPtr->sock, wrSockPtr->rateLimit);

    /*
     * Make sure we have proper content length header for
     * keep-alive/pipelining.
     */
    Ns_ConnSetLengthHeader(conn, nsend, (wrSockPtr->flags & NS_CONN_STREAM) != 0u);

    /*
     * Flush the headers
     */

    if ((conn->flags & NS_CONN_SENTHDRS) == 0u) {
        Tcl_DString    ds;

        Ns_DStringInit(&ds);
        Ns_Log(DriverDebug, "### Writer(%d): add header", fd);
        conn->flags |= NS_CONN_SENTHDRS;
        (void)Ns_CompleteHeaders(conn, nsend, 0u, &ds);

        headerSize = (size_t)Ns_DStringLength(&ds);
        if (headerSize > 0u) {
            wrSockPtr->headerString = ns_strdup(Tcl_DStringValue(&ds));
        }
        Ns_DStringFree(&ds);
    } else {
        headerSize = 0u;
    }

    if (fd != NS_INVALID_FD) {
        /* maybe add mmap support for files (fd != NS_INVALID_FD) */

        wrSockPtr->fd = fd;
        wrSockPtr->c.file.bufs = fbufs;
        wrSockPtr->c.file.nbufs = nfbufs;

        Ns_Log(DriverDebug, "### Writer(%d) tosend %" PRIdz " files %d bufsize %" PRIdz,
               fd, nsend, nfbufs, wrPtr->bufsize);

        if (unlikely(headerSize >= wrPtr->bufsize)) {
            /*
             * We have a header which is larger than bufsize; place it
             * as "leftover" and use the headerString as buffer for file
             * reads (rather rare case)
             */
            wrSockPtr->c.file.buf = (unsigned char *)wrSockPtr->headerString;
            wrSockPtr->c.file.maxsize = headerSize;
            wrSockPtr->c.file.bufsize = headerSize;
            wrSockPtr->headerString = NULL;
        } else if (headerSize > 0u) {
            /*
             * We have a header that fits into the bufsize; place it
             * as "leftover" at the end of the buffer.
             */
            wrSockPtr->c.file.buf = ns_malloc(wrPtr->bufsize);
            memcpy(wrSockPtr->c.file.buf, wrSockPtr->headerString, headerSize);
            wrSockPtr->c.file.bufsize = headerSize;
            wrSockPtr->c.file.maxsize = wrPtr->bufsize;
            ns_free(wrSockPtr->headerString);
            wrSockPtr->headerString = NULL;
        } else {
            assert(wrSockPtr->headerString == NULL);
            wrSockPtr->c.file.buf = ns_malloc(wrPtr->bufsize);
            wrSockPtr->c.file.maxsize = wrPtr->bufsize;
        }
        wrSockPtr->c.file.bufoffset = 0;
        wrSockPtr->c.file.toRead = nsend;

    } else if (bufs != NULL) {
        int   i, j, headerbufs = (headerSize > 0u ? 1 : 0);

        wrSockPtr->fd = NS_INVALID_FD;

        if (nbufs+headerbufs < UIO_SMALLIOV) {
            wrSockPtr->c.mem.bufs = wrSockPtr->c.mem.preallocated_bufs;
        } else {
            Ns_Log(DriverDebug, "NsWriterQueue: alloc %d iovecs", nbufs);
            wrSockPtr->c.mem.bufs = ns_calloc((size_t)nbufs + (size_t)headerbufs, sizeof(struct iovec));
        }
        wrSockPtr->c.mem.nbufs = nbufs+headerbufs;
        if (headerbufs != 0) {
            wrSockPtr->c.mem.bufs[0].iov_base = wrSockPtr->headerString;
            wrSockPtr->c.mem.bufs[0].iov_len  = headerSize;
        }

        if (connPtr->fmap.addr != NULL) {
            Ns_Log(DriverDebug, "NsWriterQueue: deliver fmapped %p", (void *)connPtr->fmap.addr);
            /*
             * Deliver an mmapped file, no need to copy content
             */
            for (i = 0, j=headerbufs; i < nbufs; i++, j++) {
                wrSockPtr->c.mem.bufs[j].iov_base = bufs[i].iov_base;
                wrSockPtr->c.mem.bufs[j].iov_len  = bufs[i].iov_len;
            }
            /*
             * Make a copy of the fmap structure and make clear that
             * we unmap in the writer thread.
             */
            wrSockPtr->c.mem.fmap = connPtr->fmap;
            connPtr->fmap.addr = NULL;
            /* header string will be freed via wrSockPtr->headerString */

        } else {
            /*
             * Deliver a content from iovec. The lifetime of the
             * source is unknown, we have to copy the c.
             */
            for (i = 0, j=headerbufs; i < nbufs; i++, j++) {
                wrSockPtr->c.mem.bufs[j].iov_base = ns_malloc(bufs[i].iov_len);
                wrSockPtr->c.mem.bufs[j].iov_len  = bufs[i].iov_len;
                memcpy(wrSockPtr->c.mem.bufs[j].iov_base, bufs[i].iov_base, bufs[i].iov_len);
            }
            /* header string will be freed a buf[0] */
            wrSockPtr->headerString = NULL;
        }

    } else {
        ns_free(wrSockPtr);
        return NS_ERROR;
    }

    /*
     * Add header size to total size.
     */
    nsend += headerSize;


    if (connPtr->clientData != NULL) {
        wrSockPtr->clientData = ns_strdup(connPtr->clientData);
    }
    wrSockPtr->startTime = *Ns_ConnStartTime(conn);

    /*
     * Setup streaming context before sending potentially headers.
     */

    if ((wrSockPtr->flags & NS_CONN_STREAM) != 0u) {
        wrSockPtr->doStream = NS_WRITER_STREAM_ACTIVE;
        assert(connPtr->strWriter == NULL);
        /*
         * Add a reference to the stream writer to the connection such
         * it can efficiently append to a stream when multiple output
         * operations happen. The backpointer (from the stream writer
         * to the connection is needed to clear the reference to the
         * writer in case the writer is deleted. No locks are needed,
         * since nobody can share this structure yet.
         */
        connPtr->strWriter = (NsWriterSock *)wrSockPtr;
        wrSockPtr->connPtr = connPtr;
    }

    /*
     * Tell connection, that writer handles the output (including
     * closing the connection to the client).
     */

    connPtr->flags |= NS_CONN_SENT_VIA_WRITER;
    wrSockPtr->keep = connPtr->keep > 0 ? NS_TRUE : NS_FALSE;
    wrSockPtr->size = nsend;
    Ns_Log(DriverDebug, "NsWriterQueue NS_CONN_SENT_VIA_WRITER connPtr %p",
           (void*)connPtr);

    if ((wrSockPtr->flags & NS_CONN_STREAM) == 0u) {
        Ns_Log(DriverDebug, "NsWriterQueue NS_CONN_SENT_VIA_WRITER connPtr %p clear sockPtr %p",
               (void*)connPtr, (void*)connPtr->sockPtr);
        connPtr->sockPtr = NULL;
        connPtr->flags |= NS_CONN_CLOSED;
        connPtr->nContentSent = nsend - headerSize;
    }

    /*
     * Get the next writer thread from the list, all writer requests are
     * rotated between all writer threads
     */

    Ns_MutexLock(&wrPtr->lock);
    if (wrPtr->curPtr == NULL) {
        wrPtr->curPtr = wrPtr->firstPtr;
    }
    queuePtr = wrPtr->curPtr;
    wrPtr->curPtr = wrPtr->curPtr->nextPtr;
    Ns_MutexUnlock(&wrPtr->lock);

    Ns_Log(WriterDebug, "Writer(%d): started: id=%d fd=%d, "
           "size=%" PRIdz ", flags=%X, rate %d KB/s: %s",
           wrSockPtr->sockPtr->sock,
           queuePtr->id, wrSockPtr->fd,
           nsend, wrSockPtr->flags,
           wrSockPtr->rateLimit,
           connPtr->request.line);

    /*
     * Now add new writer socket to the writer thread's queue
     */
    wrSockPtr->queuePtr = queuePtr;

    Ns_MutexLock(&queuePtr->lock);
    if (queuePtr->sockPtr == NULL) {
        trigger = NS_TRUE;
    }

    Push(wrSockPtr, queuePtr->sockPtr);
    Ns_MutexUnlock(&queuePtr->lock);

    /*
     * Wake up writer thread
     */

    if (trigger) {
        SockTrigger(queuePtr->pipe[1]);
    }

    return NS_OK;
}