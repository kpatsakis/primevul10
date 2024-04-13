DriverKeep(Sock *sockPtr)
{
    Ns_DriverKeepProc *keepProc;
    bool              result;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    keepProc = sockPtr->drvPtr->keepProc;
    if (keepProc == NULL)  {
        result = NS_FALSE;
    } else {
        result = (keepProc)((Ns_Sock *) sockPtr);
    }
    return result;
}