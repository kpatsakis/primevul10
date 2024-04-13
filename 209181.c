SockRead(Sock *sockPtr, int spooler, const Ns_Time *timePtr)
{
    const Driver       *drvPtr;
    Request      *reqPtr;
    Tcl_DString  *bufPtr;
    struct iovec  buf;
    char          tbuf[16384];
    size_t        buflen, nread;
    ssize_t       n;
    SockState     resultState;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    drvPtr = sockPtr->drvPtr;

    tbuf[0] = '\0';

    /*
     * In case of "keepwait", the accept time is not meaningful and
     * reset to 0. In such cases, update "acceptTime" to the actual
     * begin of a request. This part is intended for async drivers.
     */
    if (sockPtr->acceptTime.sec == 0) {
        assert(timePtr != NULL);
        sockPtr->acceptTime = *timePtr;
    }

    /*
     * Initialize request structure if needed.
     */
    if (sockPtr->reqPtr == NULL) {
        RequestNew(sockPtr);
    }

    /*
     * On the first read, attempt to read-ahead "bufsize" bytes.
     * Otherwise, read only the number of bytes left in the
     * content.
     */

    reqPtr = sockPtr->reqPtr;
    bufPtr = &reqPtr->buffer;
    if (reqPtr->length == 0u) {
        nread = drvPtr->bufsize;
    } else {
        nread = reqPtr->length - reqPtr->avail;
    }

    /*
     * Grow the buffer to include space for the next bytes.
     */

    buflen = (size_t)bufPtr->length;
    n = (ssize_t)(buflen + nread);
    if (unlikely(n > drvPtr->maxinput)) {
        n = (ssize_t)drvPtr->maxinput;
        nread = (size_t)n - buflen;
        if (nread == 0u) {
            Ns_Log(DriverDebug, "SockRead: maxinput reached %" TCL_LL_MODIFIER "d",
                   drvPtr->maxinput);
            return SOCK_ERROR;
        }
    }

    /*
     * Use temp file for content larger than "readahead" bytes.
     */
#ifndef _WIN32
    if (reqPtr->coff > 0u                     /* We are in the content part (after the header) */
        && !reqPtr->chunkStartOff             /* Never spool chunked encoded data since we decode in memory */
        && reqPtr->length > (size_t)drvPtr->readahead /* We need more data */
        && sockPtr->tfd <= 0                  /* We have no spool fd */
        ) {
        const DrvSpooler *spPtr = &drvPtr->spooler;

        Ns_Log(DriverDebug, "SockRead: require tmp file for content spooling (length %" PRIuz" > readahead "
               "%" TCL_LL_MODIFIER "d)",
               reqPtr->length, drvPtr->readahead);

        /*
         * In driver mode send this Sock to the spooler thread if
         * it is running
         */

        if (spooler == 0 && spPtr->threads > 0) {
            return SOCK_SPOOL;
        }

        /*
         * If "maxupload" is specified and content size exceeds the configured
         * values, spool uploads into normal temp file (not deleted).  We do
         * not want to map such large files into memory.
         */
        if (drvPtr->maxupload > 0
            && reqPtr->length > (size_t)drvPtr->maxupload
            ) {
            size_t tfileLength = strlen(drvPtr->uploadpath) + 16u;

            sockPtr->tfile = ns_malloc(tfileLength);
            snprintf(sockPtr->tfile, tfileLength, "%s/%d.XXXXXX", drvPtr->uploadpath, sockPtr->sock);
            sockPtr->tfd = ns_mkstemp(sockPtr->tfile);
            if (sockPtr->tfd == NS_INVALID_FD) {
                Ns_Log(Error, "SockRead: cannot create spool file with template '%s': %s",
                       sockPtr->tfile, strerror(errno));
            }
        } else {
            /*
             * Get a temporary fd. These FDs are used for mmapping.
             */

            sockPtr->tfd = Ns_GetTemp();
        }

        if (unlikely(sockPtr->tfd == NS_INVALID_FD)) {
            Ns_Log(DriverDebug, "SockRead: spool fd invalid");
            return SOCK_ERROR;
        }

        n = (ssize_t)((size_t)bufPtr->length - reqPtr->coff);
        assert(n >= 0);
        if (ns_write(sockPtr->tfd, bufPtr->string + reqPtr->coff, (size_t)n) != n) {
            return SOCK_WRITEERROR;
        }
        Tcl_DStringSetLength(bufPtr, 0);
    }
#endif
    if (sockPtr->tfd > 0) {
        buf.iov_base = tbuf;
        buf.iov_len = MIN(nread, sizeof(tbuf));
    } else {
        Tcl_DStringSetLength(bufPtr, (int)(buflen + nread));
        buf.iov_base = bufPtr->string + reqPtr->woff;
        buf.iov_len = nread;
    }

    if (reqPtr->leftover > 0u) {
        /*
         * There is some leftover in the buffer, don't read but take the
         * leftover instead as input.
         */
        n = (ssize_t)reqPtr->leftover;
        reqPtr->leftover = 0u;
        buflen = 0u;
        Ns_Log(DriverDebug, "SockRead receive from leftover %" PRIdz " bytes", n);
    } else {
        /*
         * Receive actually some data from the driver.
         */
        n = NsDriverRecv(sockPtr, &buf, 1, NULL);
        Ns_Log(DriverDebug, "SockRead receive from network %" PRIdz " bytes sockState %.2x",
               n, (int)sockPtr->recvSockState);
    }

    {
        Ns_SockState nsSockState = sockPtr->recvSockState;
    /*
     * The nsSockState has one of the following values, when provided:
     *
     *      NS_SOCK_READ, NS_SOCK_DONE, NS_SOCK_AGAIN, NS_SOCK_EXCEPTION,
     *      NS_SOCK_TIMEOUT
     */
        switch (nsSockState) {
        case NS_SOCK_TIMEOUT:  NS_FALL_THROUGH; /* fall through */
        case NS_SOCK_EXCEPTION:
            return SOCK_READERROR;
        case NS_SOCK_AGAIN:
            Tcl_DStringSetLength(bufPtr, (int)buflen);
            return SOCK_MORE;
        case NS_SOCK_DONE:
            return SOCK_CLOSE;
        case NS_SOCK_READ:
            break;

        case NS_SOCK_CANCEL:  NS_FALL_THROUGH; /* fall through */
        case NS_SOCK_EXIT:    NS_FALL_THROUGH; /* fall through */
        case NS_SOCK_INIT:    NS_FALL_THROUGH; /* fall through */
        case NS_SOCK_WRITE:
            Ns_Log(Warning, "SockRead received unexpected state %.2x from driver", nsSockState);
            return SOCK_READERROR;

        case NS_SOCK_NONE:
            /*
             * Old style state management based on "n" and "errno", which is
             * more fragile. We keep there for old-style drivers.
             */
            if (n < 0) {
                Tcl_DStringSetLength(bufPtr, (int)buflen);
                /*
                 * The driver returns -1 when the peer closed the connection, but
                 * clears the errno such we can distinguish from error conditions.
                 */
                if (errno == 0) {
                    return SOCK_CLOSE;
                }
                return SOCK_READERROR;
            }

            if (n == 0) {
                Tcl_DStringSetLength(bufPtr, (int)buflen);
                return SOCK_MORE;
            }
            break;

        }
    }

    if (sockPtr->tfd > 0) {
        if (ns_write(sockPtr->tfd, tbuf, (size_t)n) != n) {
            return SOCK_WRITEERROR;
        }
    } else {
        Tcl_DStringSetLength(bufPtr, (int)(buflen + (size_t)n));
    }

    reqPtr->woff  += (size_t)n;
    reqPtr->avail += (size_t)n;

    /*
     * This driver needs raw buffer, it is binary or non-HTTP request
     */

    if ((drvPtr->opts & NS_DRIVER_NOPARSE) != 0u) {
        return SOCK_READY;
    }

    resultState = SockParse(sockPtr);

    return resultState;
}