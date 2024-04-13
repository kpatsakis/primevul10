void NsDriverMapVirtualServers(void)
{
    Driver *drvPtr;

    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
        const Ns_Set *lset;
        size_t        j;
        Tcl_DString   ds, *dsPtr = &ds;
        const char   *path, *defserver, *moduleName;

        moduleName = drvPtr->moduleName;
        defserver  = drvPtr->defserver;

        /*
         * Check for a "/servers" section for this driver module.
         */
        path = Ns_ConfigGetPath(NULL, moduleName, "servers", (char *)0L);
        lset = Ns_ConfigGetSection(path);

        if (lset == NULL || Ns_SetSize(lset) == 0u) {
            /*
             * The driver module has no (or empty) ".../servers" section.
             * There is no mapping from host name to virtual server defined.
             */
            if (drvPtr->server == NULL) {

                /*
                 * We have a global driver module. If there is at least a
                 * default server configured, we can use this for the mapping
                 * to the default server.
                 */
                if (defserver != NULL) {
                    NsServer   *servPtr = NsGetServer(defserver);

                    Tcl_DStringInit(dsPtr);
                    ServerMapEntryAdd(dsPtr, Ns_InfoHostname(), servPtr, drvPtr, NS_TRUE);
                    Tcl_DStringFree(dsPtr);
                    Ns_Log(Notice, "Global driver has no mapping from host to server (section '%s' missing)",
                           moduleName);
                } else {
                    /*
                     * Global driver, which has no default server, and no servers section.
                     */
                    Ns_Fatal("%s: virtual servers configured,"
                             " but '%s' has no defaultserver defined", moduleName, path);
                }
            }

            continue;
        }

        /*
         * We have a ".../servers" section, the driver might be global or
         * local. It is not clear, why we need the server map for the local
         * driver, but for compatibility, we keep this.
         *
         */
        if (defserver == NULL) {
            if (drvPtr->server != NULL) {
                /*
                 * We have a local (server specific) driver.  Since the code
                 * below assumes that we have a "defserver" set, we take the
                 * actual server as defserver.
                 */
                defserver = drvPtr->server;
            } else {
                /*
                 * We have a global driver, but no defserver.
                 */
                Ns_Fatal("%s: virtual servers configured,"
                         " but '%s' has no defaultserver defined", moduleName, path);
            }
        }

        assert(defserver != NULL);

        drvPtr->defMapPtr = NULL;

        Ns_DStringInit(dsPtr);
        for (j = 0u; j < Ns_SetSize(lset); ++j) {
            const char *server  = Ns_SetKey(lset, j);
            const char *host    = Ns_SetValue(lset, j);
            NsServer   *servPtr;

            /*
             * Perform an explicit lookup of the server.
             */
            servPtr = NsGetServer(server);
            if (servPtr == NULL) {
                Ns_Log(Error, "%s: no such server: %s", moduleName, server);
            } else {
                char *writableHost, *hostName, *portStart;

                writableHost = ns_strdup(host);
                Ns_HttpParseHost(writableHost, &hostName, &portStart);

                if (portStart == NULL) {
                    Tcl_DString hostDString;

                    /*
                     * The provided host entry does NOT contain a port.
                     *
                     * Add the provided entry to the virtual server map only,
                     * when the configured port is the default port for the
                     * protocol.
                     */
                    if (drvPtr->port == drvPtr->defport) {
                        ServerMapEntryAdd(dsPtr, host, servPtr, drvPtr,
                                          STREQ(defserver, server));
                    }

                    /*
                     * Auto-add configured port: Add always an entry with the
                     * explicitly configured port of the driver.
                     */
                    Tcl_DStringInit(&hostDString);
                    Tcl_DStringAppend(&hostDString, host, -1);
                    (void) Ns_DStringPrintf(&hostDString, ":%hu", drvPtr->port);

                    ServerMapEntryAdd(dsPtr, hostDString.string, servPtr, drvPtr,
                                      STREQ(defserver, server));

                    Tcl_DStringFree(&hostDString);
                } else {
                    /*
                     * The provided host entry does contain a port.
                     *
                     * In case, the provided port is equal to the configured port
                     * of the driver, add an entry.
                     */
                    unsigned short providedPort = (unsigned short)strtol(portStart+1, NULL, 10);

                    if (providedPort == drvPtr->port) {
                        ServerMapEntryAdd(dsPtr, host, servPtr, drvPtr,
                                          STREQ(defserver, server));
                        /*
                         * In case, the provided port is equal to the default
                         * port of the driver, make sure that we have an entry
                         * without the port.
                         */
                        if (providedPort == drvPtr->defport) {
                            ServerMapEntryAdd(dsPtr, hostName, servPtr, drvPtr,
                                              STREQ(defserver, server));
                        }
                    } else {
                        Ns_Log(Warning, "%s: driver is listening on port %hu; "
                               "virtual host entry %s ignored",
                               moduleName, drvPtr->port, host);
                    }
                }
                ns_free(writableHost);
            }
        }
        Ns_DStringFree(dsPtr);

        if (drvPtr->defMapPtr == NULL) {
            fprintf(stderr, "--- Server Map: ---\n");
            Ns_SetPrint(lset);
            Ns_Fatal("%s: default server '%s' not defined in '%s'", moduleName, defserver, path);
        }
    }
}