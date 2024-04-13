DriverAccept(Sock *sockPtr, NS_SOCKET sock)
{
    socklen_t n = (socklen_t)sizeof(struct NS_SOCKADDR_STORAGE);

    NS_NONNULL_ASSERT(sockPtr != NULL);

    return (*sockPtr->drvPtr->acceptProc)((Ns_Sock *) sockPtr,
                                          sock,
                                          (struct sockaddr *) &(sockPtr->sa), &n);
}