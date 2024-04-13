NsDriverSend(Sock *sockPtr, const struct iovec *bufs, int nbufs, unsigned int flags)
{
    ssize_t       sent = -1;
    const Driver *drvPtr;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    drvPtr = sockPtr->drvPtr;

    NS_NONNULL_ASSERT(drvPtr != NULL);

    if (likely(drvPtr->sendProc != NULL)) {
        /*
         * TODO: The Ns_DriverSendProc signature should be modified
         * to omit the timeout argument. Same with recvProc().
         */
        sent = (*drvPtr->sendProc)((Ns_Sock *) sockPtr, bufs, nbufs, NULL, flags);
    } else {
        Ns_Log(Warning, "no sendProc registered for driver %s", drvPtr->threadName);
    }

    return sent;
}