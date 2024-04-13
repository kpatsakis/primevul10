SockNew(Driver *drvPtr)
{
    Sock *sockPtr;

    NS_NONNULL_ASSERT(drvPtr != NULL);

    Ns_MutexLock(&drvPtr->lock);
    sockPtr = drvPtr->sockPtr;
    if (likely(sockPtr != NULL)) {
        drvPtr->sockPtr = sockPtr->nextPtr;
        sockPtr->keep   = NS_FALSE;
    }
    Ns_MutexUnlock(&drvPtr->lock);

    if (sockPtr == NULL) {
        size_t sockSize = sizeof(Sock) + (nsconf.nextSlsId * sizeof(Ns_Callback *));
        sockPtr = ns_calloc(1u, sockSize);
        sockPtr->drvPtr = drvPtr;
    } else {
        sockPtr->tfd    = 0;
        sockPtr->taddr  = NULL;
        sockPtr->flags  = 0u;
        sockPtr->arg    = NULL;
        sockPtr->recvSockState = NS_SOCK_NONE;
    }
    return sockPtr;
}