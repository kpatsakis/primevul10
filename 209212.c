NSDriverSockNew(Tcl_Interp *interp, NS_SOCKET sock,
                 const char *protocol, const char *driverName, const char *methodName,
                 Sock **sockPtrPtr)
{
    int     result = TCL_OK;
    Driver *drvPtr;

    NS_NONNULL_ASSERT(interp != NULL);
    NS_NONNULL_ASSERT(protocol != NULL);
    NS_NONNULL_ASSERT(methodName != NULL);
    NS_NONNULL_ASSERT(sockPtrPtr != NULL);

    drvPtr = LookupDriver(interp, protocol, driverName);
    if (drvPtr == NULL) {
        result = TCL_ERROR;
    } else {
        Sock        *sockPtr;
        Tcl_DString  ds, *dsPtr = &ds;
        Request     *reqPtr;

        sockPtr = SockNew(drvPtr);
        sockPtr->servPtr = drvPtr->servPtr;
        sockPtr->sock = sock;

        RequestNew(sockPtr); // not sure if needed
        // peerAddr is missing

        Ns_GetTime(&sockPtr->acceptTime);
        reqPtr = sockPtr->reqPtr;

        Tcl_DStringInit(dsPtr);
        Ns_DStringAppend(dsPtr, methodName);
        Ns_StrToUpper(Ns_DStringValue(dsPtr));

        reqPtr->request.line = Ns_DStringExport(dsPtr);
        reqPtr->request.method = ns_strdup(methodName);
        reqPtr->request.protocol = ns_strdup(protocol);
        reqPtr->request.host = NULL;
        reqPtr->request.query = NULL;
        /* Ns_Log(Notice, "REQUEST LINE <%s>", reqPtr->request.line);*/

        *sockPtrPtr = sockPtr;
    }

    return result;
}