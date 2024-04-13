ServerMapEntryAdd(Tcl_DString *dsPtr, const char *host,
                  NsServer *servPtr, Driver *drvPtr, bool addDefaultMapEntry) {
    Tcl_HashEntry *hPtr;
    int            isNew;

    NS_NONNULL_ASSERT(dsPtr != NULL);
    NS_NONNULL_ASSERT(host != NULL);
    NS_NONNULL_ASSERT(servPtr != NULL);
    NS_NONNULL_ASSERT(drvPtr != NULL);

    hPtr = Tcl_CreateHashEntry(&drvPtr->hosts, host, &isNew);
    if (isNew != 0) {
        ServerMap *mapPtr;

        (void) Ns_DStringVarAppend(dsPtr, drvPtr->protocol, "://", host, (char *)0L);
        mapPtr = ns_malloc(sizeof(ServerMap) + (size_t)dsPtr->length);
        mapPtr->servPtr  = servPtr;
        memcpy(mapPtr->location, dsPtr->string, (size_t)dsPtr->length + 1u);

        Tcl_SetHashValue(hPtr, mapPtr);
        Ns_Log(Notice, "%s: adding virtual host entry for host <%s> location: %s mapped to server: %s",
               drvPtr->threadName, host, mapPtr->location, servPtr->server);

        if (addDefaultMapEntry) {
            drvPtr->defMapPtr = mapPtr;
        }
        /*
         * Always reset the Tcl_DString
         */
        Ns_DStringSetLength(dsPtr, 0);
    } else {
        Ns_Log(Notice, "%s: ignore duplicate virtual host entry: %s",
               drvPtr->threadName, host);
    }
}