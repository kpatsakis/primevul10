LookupDriver(Tcl_Interp *interp, const char* protocol, const char *driverName)
{
    Driver *drvPtr;

    NS_NONNULL_ASSERT(interp != NULL);
    NS_NONNULL_ASSERT(protocol != NULL);

    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
        Ns_Log(DriverDebug, "... check Driver proto <%s> server %s name %s location %s",
               drvPtr->protocol, drvPtr->server, drvPtr->threadName, drvPtr->location);

        if (STREQ(drvPtr->protocol, protocol)) {
            if (driverName == NULL) {
                /*
                 * If there is no driver name given, take the first driver
                 * with the matching protocol.
                 */
                break;
            } else if (STREQ(drvPtr->moduleName, driverName)) {
                /*
                 * The driver name (name of the loaded module) is equal
                 */
                break;
            }
        }
    }

    if (drvPtr == NULL) {
        if (driverName != NULL) {
            Ns_TclPrintfResult(interp, "no driver for protocol '%s' & driver name '%s' found", protocol, driverName);
        } else {
            Ns_TclPrintfResult(interp, "no driver for protocol '%s' found", protocol);
        }
    }
    return drvPtr;
}