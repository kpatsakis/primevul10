SockParse(Sock *sockPtr)
{
    const Tcl_DString  *bufPtr;
    const Driver       *drvPtr;
    Request            *reqPtr;
    char                save;
    SockState           result;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    drvPtr = sockPtr->drvPtr;

    NsUpdateProgress((Ns_Sock *) sockPtr);

    reqPtr = sockPtr->reqPtr;
    bufPtr = &reqPtr->buffer;

    /*
     * Scan lines (header) until start of content (body-part)
     */

    while (reqPtr->coff == 0u) {
        char *s, *e;
        size_t cnt;

        /*
         * Find the next header line.
         */
        s = bufPtr->string + reqPtr->roff;
        e = memchr(s, INTCHAR('\n'), reqPtr->avail);

        if (unlikely(e == NULL)) {
            /*
             * Input not yet newline terminated - request more data.
             */
            return SOCK_MORE;
        }

        /*
         * Check for max single line overflows.
         *
         * Previous versions if the driver returned here directly an
         * error code, which was handled via HTTP error message
         * provided via SockError(). However, the SockError() handling
         * closes the connection immediately. This has the
         * consequence, that the HTTP client might never see the error
         * message, since the request was not yet fully transmitted,
         * but it will see a "broken pipe: 13" message instead. We
         * read now the full request and return the message via
         * ConnRunRequest().
         */

        if (unlikely((e - s) > drvPtr->maxline)) {
            sockPtr->keep = NS_FALSE;
            if (reqPtr->request.line == NULL) {
                Ns_Log(DriverDebug, "SockParse: maxline reached of %d bytes",
                       drvPtr->maxline);
                sockPtr->flags = NS_CONN_REQUESTURITOOLONG;
                Ns_Log(Warning, "request line is too long (%d bytes)", (int)(e - s));
            } else {
                sockPtr->flags = NS_CONN_LINETOOLONG;
                Ns_Log(Warning, "request header line is too long (%d bytes)", (int)(e - s));
            }
        }

        /*
         * Update next read pointer to end of this line.
         */
        cnt = (size_t)(e - s) + 1u;
        reqPtr->roff  += cnt;
        reqPtr->avail -= cnt;

        /*
         * Adjust end pointer to the last content character before the line
         * terminator.
         */
        if (likely(e > s) && likely(*(e-1) == '\r')) {
            --e;
        }

        /*
         * Check for end of headers in case we have not done it yet.
         */
        if (unlikely(e == s) && (reqPtr->coff == 0u)) {
            /*
             * We are at end of headers.
             */
            reqPtr->coff = EndOfHeader(sockPtr);

            /*
             * In cases the client sent "expect: 100-continue", report back that
             * everything is fine with the headers.
             */
            if ((sockPtr->flags & NS_CONN_CONTINUE) != 0u) {

                Ns_Log(Ns_LogRequestDebug, "honoring 100-continue");

                /*
                 * In case, the request entity (body) was too large, we can
                 * return immediately the error message, when the client has
                 * flagged this via "Expect:". Otherwise we have to read the
                 * full request (although it is too large) to drain the
                 * channel. Otherwise, the server might close the connection
                 * *before* it has received full request with its body from
                 * the client. We just keep the flag and let
                 * Ns_ConnRunRequest() handle the error message.
                 */
                if ((sockPtr->flags & NS_CONN_ENTITYTOOLARGE) != 0u) {
                    Ns_Log(Ns_LogRequestDebug, "100-continue: entity too large");

                    return SOCK_ENTITYTOOLARGE;

                    /*
                     * We have no other error message flagged (future ones
                     * have to be handled here).
                     */
                } else {
                    struct iovec iov[1];
                    ssize_t      sent;

                    /*
                     * Reply with "100 continue".
                     */
                    Ns_Log(Ns_LogRequestDebug, "100-continue: reply CONTINUE");

                    iov[0].iov_base = (char *)"HTTP/1.1 100 Continue\r\n\r\n";
                    iov[0].iov_len = strlen(iov[0].iov_base);

                    sent = Ns_SockSendBufs((Ns_Sock *)sockPtr, iov, 1,
                                           NULL, 0u);
                    if (sent != (ssize_t)iov[0].iov_len) {
                        Ns_Log(Warning, "could not deliver response: 100 Continue");
                        /*
                         * Should we bail out here?
                         */
                    }
                }
            }
        } else {
            /*
             * We have the request-line or a header line to process.
             */
            save = *e;
            *e = '\0';

            if (unlikely(reqPtr->request.line == NULL)) {
                /*
                 * There is no request-line set. The received line must the
                 * the request-line.
                 */
                Ns_Log(DriverDebug, "SockParse (%d): parse request line <%s>", sockPtr->sock, s);

                if (Ns_ParseRequest(&reqPtr->request, s) == NS_ERROR) {
                    /*
                     * Invalid request.
                     */
                    return SOCK_BADREQUEST;
                }

                /*
                 * HTTP 0.9 did not have a HTTP-version number or request headers
                 * and no empty line terminating the request header.
                 */
                if (unlikely(reqPtr->request.version < 1.0)) {
                    /*
                     * Pre-HTTP/1.0 request.
                     */
                    reqPtr->coff = reqPtr->roff;
                    Ns_Log(Notice, "pre-HTTP/1.0 request <%s>", reqPtr->request.line);
                }

            } else if (Ns_ParseHeader(reqPtr->headers, s, Preserve) != NS_OK) {
                /*
                 * Invalid header.
                 */
                return SOCK_BADHEADER;

            } else {
                /*
                 * Check for max number of headers
                 */
                if (unlikely(Ns_SetSize(reqPtr->headers) > (size_t)drvPtr->maxheaders)) {
                    Ns_Log(DriverDebug, "SockParse (%d): maxheaders reached of %d bytes",
                           sockPtr->sock, drvPtr->maxheaders);
                    return SOCK_TOOMANYHEADERS;
                }
            }

            *e = save;
        }
    }

    if (unlikely(reqPtr->request.line == NULL)) {
        /*
         * We are at end of headers, but we have not parsed a request line
         * (maybe just two linefeeds).
         */
        return SOCK_BADREQUEST;
    }


    /*
     * We are in the request body.
     */
    assert(reqPtr->coff > 0u);
    assert(reqPtr->request.line != NULL);

    /*
     * Check if all content has arrived.
     */
    Ns_Log(Debug, "=== length < avail (length %" PRIuz
           ", avail %" PRIuz ") tfd %d tfile %p chunkStartOff %" PRIuz,
           reqPtr->length, reqPtr->avail, sockPtr->tfd,
           (void *)sockPtr->tfile, reqPtr->chunkStartOff);

    if (reqPtr->chunkStartOff != 0u) {
        /*
         * Chunked encoding was provided.
         */
        SockState chunkState;
        size_t    currentContentLength;

        chunkState = ChunkedDecode(reqPtr, NS_TRUE);
        currentContentLength = reqPtr->chunkWriteOff - reqPtr->coff;

        /*
         * A chunk might be complete, but it might not be the last
         * chunk from the client. The best thing would be to be able
         * to read until EOF here. In cases, where the (optional)
         * "expectedLength" was provided by the client, we terminate
         * depending on that information
         */
        if ((chunkState == SOCK_MORE)
            || (reqPtr->expectedLength != 0u && currentContentLength < reqPtr->expectedLength)) {
            /*
             * ChunkedDecode wants more data.
             */
            return SOCK_MORE;

        } else if (chunkState != SOCK_READY) {
            return chunkState;
        }
        /*
         * ChunkedDecode has enough data.
         */
        reqPtr->length = (size_t)currentContentLength;
    }

    if (reqPtr->avail < reqPtr->length) {
        Ns_Log(DriverDebug, "SockRead wait for more input");
        /*
         * Wait for more input.
         */
        return SOCK_MORE;
    }

    Ns_Log(Dev, "=== all required data is available (avail %" PRIuz", length %" PRIuz ", "
           "readahead %" TCL_LL_MODIFIER "d maxupload %" TCL_LL_MODIFIER "d) tfd %d",
           reqPtr->avail, reqPtr->length, drvPtr->readahead, drvPtr->maxupload,
           sockPtr->tfd);

    /*
     * We have all required data in the receive buffer or in a temporary file.
     *
     * - Uploads > "readahead": these are put into temporary files.
     *
     * - Uploads > "maxupload": these are put into temporary files
     *   without mmapping, no content parsing will be performed in memory.
     */
    result = SOCK_READY;

    if (sockPtr->tfile != NULL) {
        reqPtr->content = NULL;
        reqPtr->next = NULL;
        reqPtr->avail = 0u;
        Ns_Log(DriverDebug, "content spooled to file: size %" PRIdz ", file %s",
               reqPtr->length, sockPtr->tfile);
        /*
         * Nothing more to do, return via SOCK_READY;
         */
    } else {

        /*
         * Uploads < "maxupload" are spooled to files and mmapped in order to
         * provide the usual interface via [ns_conn content].
         */
        if (sockPtr->tfd > 0) {
#ifdef _WIN32
            /*
             * For _WIN32, tfd should never be set, since tfd-spooling is not
             * implemented for windows.
             */
            assert(0);
#else
            int prot = PROT_READ | PROT_WRITE;
            /*
             * Add a byte to make sure, the string termination with \0 below falls
             * always into the mmapped area. On some older OSes this might lead to
             * crashes when we hitting page boundaries.
             */
            ssize_t rc = ns_write(sockPtr->tfd, "\0", 1);
            if (rc == -1) {
                Ns_Log(Error, "socket: could not append terminating 0-byte");
            }
            sockPtr->tsize = reqPtr->length + 1;
            sockPtr->taddr = mmap(0, sockPtr->tsize, prot, MAP_PRIVATE,
                                  sockPtr->tfd, 0);
            if (sockPtr->taddr == MAP_FAILED) {
                sockPtr->taddr = NULL;
                result = SOCK_ERROR;

            } else {
                reqPtr->content = sockPtr->taddr;
                Ns_Log(Debug, "content spooled to mmapped file: readahead=%"
                       TCL_LL_MODIFIER "d, filesize=%" PRIdz,
                       drvPtr->readahead, sockPtr->tsize);
            }
#endif
        } else {
            /*
             * Set the content the begin of the remaining buffer (content offset).
             * This happens as well when reqPtr->contentLength is 0, but it is
             * needed for chunked input processing.
             */
            reqPtr->content = bufPtr->string + reqPtr->coff;
        }
        reqPtr->next = reqPtr->content;

        /*
         * Add a terminating null character. The content might be from the receive
         * buffer (Tcl_DString) or from the mmapped file. Non-mmapped files are handled
         * above.
         */
        if (reqPtr->length > 0u) {
            Ns_Log(DriverDebug, "SockRead adds null terminating character at content[%" PRIuz "]", reqPtr->length);

            reqPtr->savedChar = reqPtr->content[reqPtr->length];
            reqPtr->content[reqPtr->length] = '\0';
            if (sockPtr->taddr == NULL) {
                LogBuffer(DriverDebug, "UPDATED BUFFER", sockPtr->reqPtr->buffer.string, (size_t)reqPtr->buffer.length);
            }
        }
    }

    return result;
}