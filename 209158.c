SockTimeout(Sock *sockPtr, const Ns_Time *nowPtr, const Ns_Time *timeout)
{
    NS_NONNULL_ASSERT(sockPtr != NULL);
    sockPtr->timeout = *nowPtr;
    Ns_IncrTime(&sockPtr->timeout, timeout->sec, timeout->usec);
}