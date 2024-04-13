NsSockClose(Sock *sockPtr, int keep)
{
    Driver *drvPtr;
    bool    trigger = NS_FALSE;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    drvPtr = sockPtr->drvPtr;

    Ns_Log(DriverDebug, "NsSockClose sockPtr %p (%d) keep %d",
           (void *)sockPtr, ((Ns_Sock*)sockPtr)->sock, keep);

    SockClose(sockPtr, keep);
    /*
     * Free the request, unless it is from a non-HTTP driver (who might not
     * fill out the request structure).
     */
    if (sockPtr->reqPtr != NULL) {
        Ns_Log(DriverDebug, "NsSockClose calls RequestFree");
        RequestFree(sockPtr);
    }

    Ns_MutexLock(&drvPtr->lock);
    if (drvPtr->closePtr == NULL) {
        trigger = NS_TRUE;
    }
    sockPtr->nextPtr = drvPtr->closePtr;
    drvPtr->closePtr = sockPtr;
    Ns_MutexUnlock(&drvPtr->lock);

    if (trigger) {
        SockTrigger(drvPtr->trigger[1]);
    }
}