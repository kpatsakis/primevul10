SockQueue(Sock *sockPtr, const Ns_Time *timePtr)
{
    Ns_ReturnCode result;

    NS_NONNULL_ASSERT(sockPtr != NULL);
    /*
     *  Verify the conditions. Request struct must exist already.
     */
    assert(sockPtr->reqPtr != NULL);

    SockSetServer(sockPtr);
    assert(sockPtr->servPtr != NULL);

    /*
     *  Actual queueing, if not ready spool to the waiting list.
     */
    if (!NsQueueConn(sockPtr, timePtr)) {
        result = NS_TIMEOUT;
    } else {
        result = NS_OK;
    }

    return result;
}