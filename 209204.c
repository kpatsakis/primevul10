EndOfHeader(Sock *sockPtr)
{
    Request      *reqPtr;
    const char   *s;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    reqPtr = sockPtr->reqPtr;
    assert(reqPtr != NULL);

    reqPtr->chunkStartOff = 0u;

    /*
     * Check for "expect: 100-continue" and clear flag in case we have
     * pipelining.
     */
    sockPtr->flags &= ~(NS_CONN_CONTINUE);
    s = Ns_SetIGet(reqPtr->headers, "expect");
    if (s != NULL) {
        if (*s == '1' && *(s+1) == '0' && *(s+2) == '0' && *(s+3) == '-') {
            char *dup = ns_strdup(s+4);

            Ns_StrToLower(dup);
            if (STREQ(dup, "continue")) {
                sockPtr->flags |= NS_CONN_CONTINUE;
            }
            ns_free(dup);
        }
    }

    /*
     * Handle content-length, which might be provided or not.
     * Clear length specific error flags.
     */
    sockPtr->flags &= ~(NS_CONN_ENTITYTOOLARGE);
    s = Ns_SetIGet(reqPtr->headers, "content-length");
    if (s == NULL) {
        s = Ns_SetIGet(reqPtr->headers, "Transfer-Encoding");

        if (s != NULL) {
            /* Lower case is in the standard, capitalized by macOS */
            if (STREQ(s, "chunked") || STREQ(s, "Chunked")) {
                Tcl_WideInt expected;

                reqPtr->chunkStartOff = reqPtr->roff;
                reqPtr->chunkWriteOff = reqPtr->chunkStartOff;
                reqPtr->contentLength = 0u;

                /*
                 * We need reqPtr->expectedLength for safely terminating read loop.
                 */
                s = Ns_SetIGet(reqPtr->headers, "X-Expected-Entity-Length");

                if ((s != NULL)
                    && (Ns_StrToWideInt(s, &expected) == NS_OK)
                    && (expected > 0) ) {
                    reqPtr->expectedLength = (size_t)expected;
                }
                s = NULL;
            }
        }
    }

    /*
     * In case a valid and meaningful was provided, the string with the
     * content length ("s") is not NULL.
     */
    if (s != NULL) {
        Tcl_WideInt length;

        if ((Ns_StrToWideInt(s, &length) == NS_OK) && (length > 0)) {
            reqPtr->length = (size_t)length;
            /*
             * Handle too large input requests.
             */
            if (reqPtr->length > (size_t)sockPtr->drvPtr->maxinput) {

                Ns_Log(Warning, "SockParse: request too large, length=%"
                       PRIdz ", maxinput=%" TCL_LL_MODIFIER "d",
                       reqPtr->length, sockPtr->drvPtr->maxinput);

                sockPtr->keep = NS_FALSE;
                sockPtr->flags |= NS_CONN_ENTITYTOOLARGE;

            }
            reqPtr->contentLength = (size_t)length;
        }
    }

    /*
     * Compression format handling: parse information from request headers
     * indicating allowed compression formats for quick access.
     *
     * Clear compression accepted flag
     */
    sockPtr->flags &= ~(NS_CONN_ZIPACCEPTED|NS_CONN_BROTLIACCEPTED);

    s = Ns_SetIGet(reqPtr->headers, "Accept-Encoding");
    if (s != NULL) {
        bool gzipAccept, brotliAccept;

        /*
         * Get allowed compression formats from "accept-encoding" headers.
         */
        NsParseAcceptEncoding(reqPtr->request.version, s, &gzipAccept, &brotliAccept);
        if (gzipAccept || brotliAccept) {
            /*
             * Don't allow compression formats for Range requests.
             */
            s = Ns_SetIGet(reqPtr->headers, "Range");
            if (s == NULL) {
                if (gzipAccept) {
                    sockPtr->flags |= NS_CONN_ZIPACCEPTED;
                }
                if (brotliAccept) {
                    sockPtr->flags |= NS_CONN_BROTLIACCEPTED;
                }
            }
        }
    }

    /*
     * Set up request length for spooling and further read operations
     */
    if (reqPtr->contentLength != 0u) {
        /*
         * Content-Length was provided, use it
         */
        reqPtr->length = reqPtr->contentLength;
    }

    return reqPtr->roff;
}