NsGetRequest(Sock *sockPtr, const Ns_Time *nowPtr)
{
    Request *reqPtr;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    /*
     * The underlying "Request" structure is allocated by RequestNew(), which
     * must be called for the "sockPtr" prior to calling this
     * function. "reqPtr" should be NULL just in error cases.
     */
    reqPtr = sockPtr->reqPtr;

    if (likely(reqPtr != NULL)) {

        if (likely(reqPtr->request.line != NULL)) {
            Ns_Log(DriverDebug, "NsGetRequest got the pre-parsed request <%s> from the driver",
                   reqPtr->request.line);

        } else if (sockPtr->drvPtr->requestProc == NULL) {
            /*
             * Non-HTTP driver can send the drvPtr->requestProc to perform
             * their own request handling.
             */
            SockState status;

            Ns_Log(DriverDebug, "NsGetRequest has to read+parse the request");
            /*
             * We have no parsed request so far. So, do it now.
             */
            do {
                Ns_Log(DriverDebug, "NsGetRequest calls SockRead");
                status = SockRead(sockPtr, 0, nowPtr);
            } while (status == SOCK_MORE);

            /*
             * If anything went wrong, clean the request provided by
             * SockRead() and flag the error by returning NULL.
             */
            if (status != SOCK_READY) {
                if (sockPtr->reqPtr != NULL) {
                    Ns_Log(DriverDebug, "NsGetRequest calls RequestFree");
                    RequestFree(sockPtr);
                }
                reqPtr = NULL;
            }
        } else {
            Ns_Log(DriverDebug, "NsGetRequest found driver specific request Proc, "
                   "probably from a non-HTTP driver");
        }
    } else {
        Ns_Log(DriverDebug, "NsGetRequest has reqPtr NULL");
    }

    return reqPtr;
}