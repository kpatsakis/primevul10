NSDriverClientOpen(Tcl_Interp *interp, const char *driverName,
                   const char *url, const char *httpMethod, const char *version,
                   const Ns_Time *timeoutPtr, Sock **sockPtrPtr)
{
    char   *protocol, *host, *portString, *path, *tail, *url2;
    int     result = TCL_OK;

    NS_NONNULL_ASSERT(interp != NULL);
    NS_NONNULL_ASSERT(url != NULL);
    NS_NONNULL_ASSERT(httpMethod != NULL);
    NS_NONNULL_ASSERT(version != NULL);
    NS_NONNULL_ASSERT(sockPtrPtr != NULL);

    url2 = ns_strdup(url);

    /*
     * We need here a fully qualified URL, otherwise raise an error
     */
    if (unlikely(Ns_ParseUrl(url2, &protocol, &host, &portString, &path, &tail) != NS_OK)
        || protocol == NULL || host == NULL || path == NULL || tail == NULL) {
        Ns_Log(Notice, "driver: invalid URL '%s' passed to NSDriverClientOpen", url2);
        result = TCL_ERROR;

    } else {
        Driver        *drvPtr;
        unsigned short portNr = 0u; /* make static checker happy */

        assert(protocol != NULL);
        assert(host != NULL);
        assert(path != NULL);
        assert(tail != NULL);

        /*
         * Find a matching driver for the specified protocol and optionally
         * the specified driver name.
         */
        drvPtr = LookupDriver(interp, protocol, driverName);
        if (drvPtr == NULL) {
            result = TCL_ERROR;

        } else if (portString != NULL) {
            portNr = (unsigned short) strtol(portString, NULL, 10);

        } else if (drvPtr->defport != 0u) {
            /*
             * Get the default port from the driver structure;
             */
            portNr = drvPtr->defport;

        } else {
            Ns_TclPrintfResult(interp, "no default port for protocol '%s' defined", protocol);
            result = TCL_ERROR;
        }

        if (result == TCL_OK) {
            NS_SOCKET     sock;
            Ns_ReturnCode status;

            sock = Ns_SockTimedConnect2(host, portNr, NULL, 0u, timeoutPtr, &status);

            if (sock == NS_INVALID_SOCKET) {
                Ns_SockConnectError(interp, host, portNr, status);
                result = TCL_ERROR;

            } else {
                const char    *query;
                Tcl_DString    ds, *dsPtr = &ds;
                Request       *reqPtr;
                Sock          *sockPtr;

                assert(drvPtr != NULL);

                sockPtr = SockNew(drvPtr);
                sockPtr->sock = sock;
                sockPtr->servPtr  = drvPtr->servPtr;
                if (sockPtr->servPtr == NULL) {
                    const NsInterp *itPtr = NsGetInterpData(interp);

                    sockPtr->servPtr = itPtr->servPtr;
                }

                RequestNew(sockPtr);

                Ns_GetTime(&sockPtr->acceptTime);
                reqPtr = sockPtr->reqPtr;

                Tcl_DStringInit(dsPtr);
                Ns_DStringAppend(dsPtr, httpMethod);
                Ns_StrToUpper(Ns_DStringValue(dsPtr));
                Tcl_DStringAppend(dsPtr, " /", 2);
                if (*path != '\0') {
                    if (*path == '/') {
                        path ++;
                    }
                    Tcl_DStringAppend(dsPtr, path, -1);
                    Tcl_DStringAppend(dsPtr, "/", 1);
                }
                Tcl_DStringAppend(dsPtr, tail, -1);
                Tcl_DStringAppend(dsPtr, " HTTP/", 6);
                Tcl_DStringAppend(dsPtr, version, -1);

                reqPtr->request.line = Ns_DStringExport(dsPtr);
                reqPtr->request.method = ns_strdup(httpMethod);
                reqPtr->request.protocol = ns_strdup(protocol);
                reqPtr->request.host = ns_strdup(host);
                query = strchr(tail, INTCHAR('?'));
                if (query != NULL) {
                    reqPtr->request.query = ns_strdup(query+1);
                } else {
                    reqPtr->request.query = NULL;
                }
                /*Ns_Log(Notice, "REQUEST LINE <%s> query <%s>", reqPtr->request.line, reqPtr->request.query);*/

                *sockPtrPtr = sockPtr;
            }
        }
    }
    ns_free(url2);

    return result;
}