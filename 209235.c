NsDriverRecv(Sock *sockPtr, struct iovec *bufs, int nbufs, Ns_Time *timeoutPtr)
{
    ssize_t       result;
    const Driver *drvPtr;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    drvPtr = sockPtr->drvPtr;

    if (likely(drvPtr->recvProc != NULL)) {
        result = (*drvPtr->recvProc)((Ns_Sock *) sockPtr, bufs, nbufs, timeoutPtr, 0u);
    } else {
        Ns_Log(Warning, "driver: no recvProc registered for driver %s", drvPtr->threadName);
        result = -1;
    }

    return result;
}