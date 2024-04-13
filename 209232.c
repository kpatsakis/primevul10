NsDriverSendFile(Sock *sockPtr, Ns_FileVec *bufs, int nbufs, unsigned int flags)
{
    ssize_t       sent = -1;
    const Driver *drvPtr;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    NS_NONNULL_ASSERT(bufs != NULL);

    drvPtr = sockPtr->drvPtr;

    NS_NONNULL_ASSERT(drvPtr != NULL);

    if (drvPtr->sendFileProc != NULL) {
        /*
         * TODO: The Ns_DriverSendFileProc signature should be modified
         * to omit the timeout argument.
         */
        sent = (*drvPtr->sendFileProc)((Ns_Sock *)sockPtr, bufs, nbufs, NULL, flags);
    } else {
        sent = Ns_SockSendFileBufs((Ns_Sock *)sockPtr, bufs, nbufs, flags);
    }

    return sent;
}