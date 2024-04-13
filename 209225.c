Ns_DriverInit(const char *server, const char *module, const Ns_DriverInitData *init)
{
    Ns_ReturnCode  status = NS_OK;
    NsServer      *servPtr = NULL;
    bool           alreadyInitialized = NS_FALSE;

    NS_NONNULL_ASSERT(module != NULL);
    NS_NONNULL_ASSERT(init != NULL);

    /*
     * If a server is provided, servPtr must be set.
     */
    if (server != NULL) {
        servPtr = NsGetServer(server);
        if (unlikely(servPtr == NULL)) {
            Ns_Log(Bug, "cannot lookup server structure for server: %s", module);
            status = NS_ERROR;
        }
    } else {
        alreadyInitialized = DriverModuleInitialized(module);
    }

    /*
     * Check versions of drivers.
     */
    if (status == NS_OK && init->version < NS_DRIVER_VERSION_4) {
        Ns_Log(Warning, "%s: driver version is too old (version %d), Version 4 is recommended",
               module, init->version);
    }
#ifdef HAVE_IPV6
    if (status == NS_OK && init->version < NS_DRIVER_VERSION_3) {
        Ns_Log(Error, "%s: driver version is too old (version %d) and does not support IPv6",
               module, init->version);
        status = NS_ERROR;
    }
#endif
    if (status == NS_OK && init->version < NS_DRIVER_VERSION_2) {
        Ns_Log(Error, "%s: version field of driver is invalid: %d",
               module, init->version);
        status = NS_ERROR;
    }

    if (!alreadyInitialized && status == NS_OK) {
        const char *path, *host, *address, *defserver;
        bool        noHostNameGiven;
        int         nrDrivers, nrBindaddrs = 0, result;
        Ns_Set     *set;
        Tcl_Obj    *bindaddrsObj, **objv;

        path = ((init->path != NULL) ? init->path : Ns_ConfigGetPath(server, module, (char *)0L));

        set = Ns_ConfigCreateSection(path);

        /*
         * Determine the "defaultserver" the "hostname" / "address" for
         * binding to and/or the HTTP location string.
         */
        defserver = Ns_ConfigGetValue(path, "defaultserver");

        address = Ns_ConfigGetValue(path, "address");
        host = Ns_ConfigGetValue(path, "hostname");
        noHostNameGiven = (host == NULL);

        /*
         * If the listen address was not specified, attempt to determine it
         * through a DNS lookup of the specified hostname or the server's
         * primary hostname.
         */
        if (address == NULL) {
            Tcl_DString  ds;

            Tcl_DStringInit(&ds);
            if (noHostNameGiven) {
                host = Ns_InfoHostname();
            }

            if (Ns_GetAllAddrByHost(&ds, host) == NS_TRUE) {
                address = ns_strdup(Tcl_DStringValue(&ds));
                if (path != NULL) {
                    Ns_SetUpdate(set, "address", address);
                }
                Ns_Log(Notice, "no address given, obtained address '%s' from host name %s", address, host);

            }
            Tcl_DStringFree(&ds);
        }

        if (address == NULL) {
            address = NS_IP_UNSPECIFIED;
            Ns_Log(Notice, "no address given, set address to unspecified address %s", address);
        }

        bindaddrsObj = Tcl_NewStringObj(address, -1);
        result = Tcl_ListObjGetElements(NULL, bindaddrsObj, &nrBindaddrs, &objv);
        if (result != TCL_OK || nrBindaddrs < 1 || nrBindaddrs >= MAX_LISTEN_ADDR_PER_DRIVER) {
            Ns_Fatal("%s: bindaddrs '%s' is not a valid Tcl list containing addresses (max %d)",
                     module, address, MAX_LISTEN_ADDR_PER_DRIVER);
        }
        Tcl_IncrRefCount(bindaddrsObj);

        /*
         * If the hostname was not specified and not determined by the lookup
         * above, set it to the first specified or derived IP address string.
         */
        if (host == NULL) {
            host = ns_strdup(Tcl_GetString(objv[0]));
        }

        if (noHostNameGiven && host != NULL && path != NULL) {
            Ns_SetUpdate(set, "hostname", host);
        }
        Tcl_DecrRefCount(bindaddrsObj);

        /*
         * Get configured number of driver threads.
         */
        nrDrivers = Ns_ConfigIntRange(path, "driverthreads", 1, 1, 64);
        if (nrDrivers > 1) {
#if !defined(SO_REUSEPORT)
            Ns_Log(Warning,
                   "server %s module %s requests %d driverthreads, but is not supported by the operating system",
                   server, module, nrDrivers);
            Ns_SetUpdate(set, "driverthreads", "1");
            nrDrivers = 1;
#endif
        }

        /*
         * The common parameters are determined, create the driver thread(s)
         */
        {
            size_t maxModuleNameLength = strlen(module) + (size_t)TCL_INTEGER_SPACE + 1u;
            char  *moduleName = ns_malloc(maxModuleNameLength);
            int    i;

            if (host == NULL) {
                host = Ns_InfoHostname();
            }

            for (i = 0; i < nrDrivers; i++) {
                snprintf(moduleName, maxModuleNameLength, "%s:%d", module, i);
                status = DriverInit(server, module, moduleName, init,
                                    servPtr, path,
                                    address, defserver, host);
                if (status != NS_OK) {
                    break;
                }
            }
            ns_free(moduleName);
        }
    }

    return status;
}