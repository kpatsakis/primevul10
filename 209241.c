SockError(Sock *sockPtr, SockState reason, int err)
{
    const char *errMsg = NULL;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    switch (reason) {
    case SOCK_READY:
    case SOCK_SPOOL:
    case SOCK_MORE:
    case SOCK_CLOSE:
    case SOCK_CLOSETIMEOUT:
        /* This is normal, never log. */
        break;

    case SOCK_READTIMEOUT:
        /*
         * For this case, whether this is acceptable or not
         * depends upon whether this sock was a keep-alive
         * that we were allowing to 'linger'.
         */
        if (!sockPtr->keep) {
            errMsg = "Timeout during read";
        }
        break;

    case SOCK_WRITETIMEOUT:
        errMsg = "Timeout during write";
        break;

    case SOCK_READERROR:
        errMsg = "Unable to read request";
        break;

    case SOCK_WRITEERROR:
        errMsg = "Unable to write request";
        break;

    case SOCK_SHUTERROR:
        errMsg = "Unable to shutdown socket";
        break;

    case SOCK_BADREQUEST:
        errMsg = "Bad Request";
        SockSendResponse(sockPtr, 400, errMsg);
        break;

    case SOCK_TOOMANYHEADERS:
        errMsg = "Too Many Request Headers";
        SockSendResponse(sockPtr, 414, errMsg);
        break;

    case SOCK_BADHEADER:
        errMsg = "Invalid Request Header";
        SockSendResponse(sockPtr, 400, errMsg);
        break;

    case SOCK_ENTITYTOOLARGE:
        errMsg = "Request Entity Too Large";
        SockSendResponse(sockPtr, 413, errMsg);
        break;

    case SOCK_ERROR:
        errMsg = "Unknown Error";
        SockSendResponse(sockPtr, 400, errMsg);
        break;
    }

    if (errMsg != NULL) {
        char ipString[NS_IPADDR_SIZE];

        Ns_Log(DriverDebug, "SockError: %s (%d: %s), sock: %d, peer: [%s]:%d, request: %.99s",
               errMsg,
               err, (err != 0) ? strerror(err) : NS_EMPTY_STRING,
               sockPtr->sock,
               ns_inet_ntop((struct sockaddr *)&(sockPtr->sa), ipString, sizeof(ipString)),
               Ns_SockaddrGetPort((struct sockaddr *)&(sockPtr->sa)),
               (sockPtr->reqPtr != NULL) ? sockPtr->reqPtr->buffer.string : NS_EMPTY_STRING);
    }
}