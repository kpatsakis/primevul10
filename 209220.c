DriverClose(Sock *sockPtr)
{
    NS_NONNULL_ASSERT(sockPtr != NULL);
    (*sockPtr->drvPtr->closeProc)((Ns_Sock *) sockPtr);
}