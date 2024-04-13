SockPoll(Sock *sockPtr, short type, PollData *pdata)
{
    NS_NONNULL_ASSERT(sockPtr != NULL);
    NS_NONNULL_ASSERT(pdata != NULL);

    sockPtr->pidx = PollSet(pdata, sockPtr->sock, type, &sockPtr->timeout);
}