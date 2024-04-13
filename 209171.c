SockRelease(Sock *sockPtr, SockState reason, int err)
{
    Driver *drvPtr;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    Ns_Log(DriverDebug, "SockRelease reason %s err %d (sock %d)",
           GetSockStateName(reason), err, sockPtr->sock);

    drvPtr = sockPtr->drvPtr;
    assert(drvPtr != NULL);

    SockError(sockPtr, reason, err);

    if (sockPtr->sock != NS_INVALID_SOCKET) {
        SockClose(sockPtr, (int)NS_FALSE);
    } else {
        Ns_Log(DriverDebug, "SockRelease bypasses SockClose, since we have an invalid socket");
    }
    NsSlsCleanup(sockPtr);

    drvPtr->queuesize--;

    if (sockPtr->reqPtr != NULL) {
        Ns_Log(DriverDebug, "SockRelease calls RequestFree");
        RequestFree(sockPtr);
    }

    Ns_MutexLock(&drvPtr->lock);
    sockPtr->nextPtr = drvPtr->sockPtr;
    drvPtr->sockPtr  = sockPtr;
    Ns_MutexUnlock(&drvPtr->lock);
}