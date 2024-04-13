SockSendResponse(Sock *sockPtr, int code, const char *errMsg)
{
    struct iovec iov[3];
    char         header[32];
    ssize_t      sent, tosend;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    NS_NONNULL_ASSERT(errMsg != NULL);

    snprintf(header, sizeof(header), "HTTP/1.0 %d ", code);
    iov[0].iov_base = header;
    iov[0].iov_len  = strlen(header);
    iov[1].iov_base = (void *)errMsg;
    iov[1].iov_len  = strlen(errMsg);
    iov[2].iov_base = (void *)"\r\n\r\n";
    iov[2].iov_len  = 4u;
    tosend = (ssize_t)(iov[0].iov_len + iov[1].iov_len + iov[2].iov_len);
    sent = NsDriverSend(sockPtr, iov, 3, 0u);
    if (sent < tosend) {
        Ns_Log(Warning, "Driver: partial write while sending response;"
               " %" PRIdz " < %" PRIdz, sent, tosend);
    }

    /*
     * In case we have a request structure, complain the system log about
     * the bad request.
     */
    if (sockPtr->reqPtr != NULL) {
        Request     *reqPtr = sockPtr->reqPtr;
        const char  *requestLine = (reqPtr->request.line != NULL) ? reqPtr->request.line : NS_EMPTY_STRING;

        (void)ns_inet_ntop((struct sockaddr *)&(sockPtr->sa), sockPtr->reqPtr->peer, NS_IPADDR_SIZE);

        /*
         * Check, if bad request looks like a TLS handshake. If yes, there is
         * no need to print out the received buffer.
         */
        if (requestLine[0] == (char)0x16 && requestLine[1] >= 3 && requestLine[2] == 1) {
            Ns_Log(Warning, "invalid request %d (%s) from peer %s: received TLS handshake on a non-TLS connection",
                   code, errMsg, reqPtr->peer);
        } else {
            Tcl_DString  dsReqLine;

            Tcl_DStringInit(&dsReqLine);
            Ns_Log(Warning, "invalid request: %d (%s) from peer %s request '%s' offsets: read %" PRIuz
                   " write %" PRIuz " content %" PRIuz " avail %" PRIuz,
                   code, errMsg,
                   reqPtr->peer,
                   Ns_DStringAppendPrintable(&dsReqLine, NS_FALSE, requestLine, strlen(requestLine)),
                   reqPtr->roff,
                   reqPtr->woff,
                   reqPtr->coff,
                   reqPtr->avail);
            Tcl_DStringFree(&dsReqLine);

            LogBuffer(Warning, "REQ BUFFER", reqPtr->buffer.string, (size_t)reqPtr->buffer.length);
        }
    } else {
        Ns_Log(Warning, "invalid request: %d (%s) - no request information available",
               code, errMsg);
    }
}