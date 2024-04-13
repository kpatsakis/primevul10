SockAccept(Driver *drvPtr, NS_SOCKET sock, Sock **sockPtrPtr, const Ns_Time *nowPtr)
{
    Sock    *sockPtr;
    SockState sockStatus;
    NS_DRIVER_ACCEPT_STATUS status;

    NS_NONNULL_ASSERT(drvPtr != NULL);

    sockPtr = SockNew(drvPtr);

    /*
     * Accept the new connection.
     */

    status = DriverAccept(sockPtr, sock);

    if (unlikely(status == NS_DRIVER_ACCEPT_ERROR)) {
        sockStatus = SOCK_ERROR;

        /*
         * We reach the place frequently, especially on Linux, when we try to
         * accept multiple connection in one sweep. Usually, the errno is
         * EAGAIN.
         */

        Ns_MutexLock(&drvPtr->lock);
        sockPtr->nextPtr = drvPtr->sockPtr;
        drvPtr->sockPtr = sockPtr;
        Ns_MutexUnlock(&drvPtr->lock);
        sockPtr = NULL;

    } else {
        sockPtr->acceptTime = *nowPtr;
        drvPtr->queuesize++;

        if (status == NS_DRIVER_ACCEPT_DATA) {

            /*
             * If there is already data present then read it without
             * polling if we're in async mode.
             */

            if ((drvPtr->opts & NS_DRIVER_ASYNC) != 0u) {
                sockStatus = SockRead(sockPtr, 0, nowPtr);
                if ((int)sockStatus < 0) {
                    Ns_Log(DriverDebug, "SockRead returned error %s",
                           GetSockStateName(sockStatus));

                    SockRelease(sockPtr, sockStatus, errno);
                    sockStatus = SOCK_ERROR;
                    sockPtr = NULL;
                }
            } else {

                /*
                 * Queue this socket without reading, NsGetRequest() in the
                 * connection thread will perform actual reading of the
                 * request.
                 */
                sockStatus = SOCK_READY;
            }
        } else if (status == NS_DRIVER_ACCEPT_QUEUE) {

            /*
             *  We need to call RequestNew() to make sure socket has request
             *  structure allocated, otherwise NsGetRequest() will call
             *  SockRead() which is not what this driver wants.
             */
            if (sockPtr->reqPtr == NULL) {
                RequestNew(sockPtr);
            }
            sockStatus = SOCK_READY;
        } else {
            sockStatus = SOCK_MORE;
        }
    }

    *sockPtrPtr = sockPtr;

    return sockStatus;
}