DriverInit(const char *server, const char *moduleName, const char *threadName,
           const Ns_DriverInitData *init,
           NsServer *servPtr, const char *path,
           const char *bindaddrs, const char *defserver, const char *host)
{
    const char     *defproto;
    Driver         *drvPtr;
    DrvWriter      *wrPtr;
    DrvSpooler     *spPtr;
    int             i;
    unsigned short  defport;

    NS_NONNULL_ASSERT(threadName != NULL);
    NS_NONNULL_ASSERT(init != NULL);
    NS_NONNULL_ASSERT(path != NULL);
    NS_NONNULL_ASSERT(bindaddrs != NULL);
    NS_NONNULL_ASSERT(host != NULL);

    /*
     * Set the protocol and port defaults.
     */
    if (init->protocol != NULL) {
        defproto = init->protocol;
        defport = init->defaultPort;
    } else {
        defproto = "unknown";
        defport = 0u;
    }
    Ns_Log(DriverDebug, "DriverInit server <%s> threadName %s proto %s port %hu",
           server, threadName, defproto, defport);

    /*
     * Allocate a new driver instance and set configurable parameters.
     */

    drvPtr = ns_calloc(1u, sizeof(Driver));

    Ns_MutexInit(&drvPtr->lock);
    Ns_MutexSetName2(&drvPtr->lock, "ns:drv", threadName);

    Ns_MutexInit(&drvPtr->spooler.lock);
    Ns_MutexSetName2(&drvPtr->spooler.lock, "ns:drv:spool", threadName);

    Ns_MutexInit(&drvPtr->writer.lock);
    Ns_MutexSetName2(&drvPtr->writer.lock, "ns:drv:writer", threadName);

    if (ns_sockpair(drvPtr->trigger) != 0) {
        Ns_Fatal("ns_sockpair() failed: %s", ns_sockstrerror(ns_sockerrno));
    }

    drvPtr->server         = server;
    drvPtr->type           = init->name;
    drvPtr->moduleName     = ns_strdup(moduleName);
    drvPtr->threadName     = ns_strdup(threadName);
    drvPtr->defserver      = defserver;
    drvPtr->listenProc     = init->listenProc;
    drvPtr->acceptProc     = init->acceptProc;
    drvPtr->recvProc       = init->recvProc;
    drvPtr->sendProc       = init->sendProc;
    drvPtr->sendFileProc   = init->sendFileProc;
    drvPtr->keepProc       = init->keepProc;
    drvPtr->requestProc    = init->requestProc;
    drvPtr->closeProc      = init->closeProc;
    drvPtr->clientInitProc = init->clientInitProc;
    drvPtr->arg            = init->arg;
    drvPtr->opts           = init->opts;
    drvPtr->servPtr        = servPtr;
    drvPtr->defport        = defport;

    drvPtr->bufsize        = (size_t)Ns_ConfigMemUnitRange(path, "bufsize", 16384, 1024, INT_MAX);
    drvPtr->maxinput       = Ns_ConfigMemUnitRange(path, "maxinput", 1024*1024, 1024, LLONG_MAX);
    drvPtr->maxupload      = Ns_ConfigMemUnitRange(path, "maxupload", 0, 0, (Tcl_WideInt)drvPtr->maxinput);
    drvPtr->readahead      = Ns_ConfigMemUnitRange(path, "readahead", (Tcl_WideInt)drvPtr->bufsize,
                                                   (Tcl_WideInt)drvPtr->bufsize, drvPtr->maxinput);

    drvPtr->maxline        = Ns_ConfigIntRange(path, "maxline",      8192, 256, INT_MAX);
    drvPtr->maxheaders     = Ns_ConfigIntRange(path, "maxheaders",    128,   8, INT_MAX);
    drvPtr->maxqueuesize   = Ns_ConfigIntRange(path, "maxqueuesize", 1024,   1, INT_MAX);

    Ns_ConfigTimeUnitRange(path, "sendwait",
                           "30s", 1, 0, INT_MAX, 0, &drvPtr->sendwait);
    Ns_ConfigTimeUnitRange(path, "recvwait",
                           "30s", 1, 0, INT_MAX, 0, &drvPtr->recvwait);
    Ns_ConfigTimeUnitRange(path, "closewait",
                           "2s", 0, 0, INT_MAX, 0, &drvPtr->closewait);
    Ns_ConfigTimeUnitRange(path, "keepwait",
                           "5s", 0, 0, INT_MAX, 0, &drvPtr->keepwait);

    drvPtr->backlog        = Ns_ConfigIntRange(path, "backlog",       256,   1, INT_MAX);
    drvPtr->driverthreads  = Ns_ConfigIntRange(path, "driverthreads",   1,   1, 32);
    drvPtr->reuseport      = Ns_ConfigBool(path,     "reuseport",       NS_FALSE);
    drvPtr->acceptsize     = Ns_ConfigIntRange(path, "acceptsize", drvPtr->backlog, 1, INT_MAX);

    drvPtr->keepmaxuploadsize   = (size_t)Ns_ConfigMemUnitRange(path, "keepalivemaxuploadsize",
                                                            0, 0, INT_MAX);
    drvPtr->keepmaxdownloadsize = (size_t)Ns_ConfigMemUnitRange(path, "keepalivemaxdownloadsize",
                                                            0, 0, INT_MAX);
    drvPtr->recvTimeout = drvPtr->recvwait;

    Tcl_InitHashTable(&drvPtr->hosts, TCL_STRING_KEYS);

    if (drvPtr->driverthreads > 1) {
#if !defined(SO_REUSEPORT)
        drvPtr->driverthreads = 1;
        drvPtr->reuseport = NS_FALSE;
#else
        /*
         * When driver threads > 1, "reuseport" has to be active.
         */
        drvPtr->reuseport = NS_TRUE;
#endif
    }
    if (drvPtr->reuseport) {
        /*
         * Reuseport was specified
         */
#if !defined(SO_REUSEPORT)
        Ns_Log(Warning,
               "parameter %s reuseport was specified, but is not supported by the operating system",
               path);
        drvPtr->reuseport = NS_FALSE;
#endif
    }

    drvPtr->uploadpath = ns_strdup(Ns_ConfigString(path, "uploadpath", nsconf.tmpDir));

    /*
     * If activated, "maxupload" has to be at least "readahead" bytes. Tell
     * the user in case the config values are overruled.
     */
    if ((drvPtr->maxupload > 0) &&
        (drvPtr->maxupload < drvPtr->readahead)) {
        Ns_Log(Warning,
               "parameter %s maxupload % " TCL_LL_MODIFIER
               "d invalid; can be either 0 or must be >= %" TCL_LL_MODIFIER
               "d (size of readahead)",
               path, drvPtr->maxupload, drvPtr->readahead);
        drvPtr->maxupload = drvPtr->readahead;
    }

    /*
     * Determine the port and then set the HTTP location string either
     * as specified in the config file or constructed from the
     * protocol, hostname and port.
     */

    drvPtr->protocol     = ns_strdup(defproto);
    drvPtr->address      = ns_strdup(bindaddrs);
    drvPtr->port         = (unsigned short)Ns_ConfigIntRange(path, "port", (int)defport, 0, 65535);
    drvPtr->location     = Ns_ConfigGetValue(path, "location");

    if (drvPtr->location != NULL && (strstr(drvPtr->location, "://") != NULL)) {
        drvPtr->location = ns_strdup(drvPtr->location);
    } else {
        Tcl_DString ds, *dsPtr = &ds;

        Ns_DStringInit(dsPtr);
        Ns_HttpLocationString(dsPtr, drvPtr->protocol, host, drvPtr->port, defport);
        drvPtr->location = Ns_DStringExport(dsPtr);
    }

    drvPtr->nextPtr = firstDrvPtr;
    firstDrvPtr = drvPtr;

    /*
     * Add driver specific extra headers.
     */
    drvPtr->extraHeaders = Ns_ConfigSet(path, "extraheaders");

    /*
     * Check if upload spooler are enabled
     */
    spPtr = &drvPtr->spooler;
    spPtr->threads = Ns_ConfigIntRange(path, "spoolerthreads", 0, 0, 32);

    if (spPtr->threads > 0) {
        Ns_Log(Notice, "%s: enable %d spooler thread(s) "
               "for uploads >= %" TCL_LL_MODIFIER "d bytes", threadName,
               spPtr->threads, drvPtr->readahead);

        for (i = 0; i < spPtr->threads; i++) {
            SpoolerQueue *queuePtr = ns_calloc(1u, sizeof(SpoolerQueue));
            char          buffer[100];

            snprintf(buffer, sizeof(buffer), "ns:driver:spooler:%d", i);
            Ns_MutexSetName2(&queuePtr->lock, buffer, "queue");
            queuePtr->id = i;
            Push(queuePtr, spPtr->firstPtr);
        }
    } else {
        Ns_Log(Notice, "%s: enable %d spooler thread(s) ",
               threadName, spPtr->threads);
    }

    /*
     * Enable writer threads
     */

    wrPtr = &drvPtr->writer;
    wrPtr->threads = Ns_ConfigIntRange(path, "writerthreads", 0, 0, 32);

    if (wrPtr->threads > 0) {
        wrPtr->writersize = (size_t)Ns_ConfigMemUnitRange(path, "writersize",
                                                          1024*1024, 1024, INT_MAX);
        wrPtr->bufsize = (size_t)Ns_ConfigMemUnitRange(path, "writerbufsize",
                                                   8192, 512, INT_MAX);
        wrPtr->rateLimit = Ns_ConfigIntRange(path, "writerratelimit", 0, 0, INT_MAX);
        wrPtr->doStream = Ns_ConfigBool(path, "writerstreaming", NS_FALSE)
            ? NS_WRITER_STREAM_ACTIVE : NS_WRITER_STREAM_NONE;
        Ns_Log(Notice, "%s: enable %d writer thread(s) "
               "for downloads >= %" PRIdz " bytes, bufsize=%" PRIdz " bytes, HTML streaming %d",
               threadName, wrPtr->threads, wrPtr->writersize, wrPtr->bufsize, wrPtr->doStream);

        for (i = 0; i < wrPtr->threads; i++) {
            SpoolerQueue *queuePtr = ns_calloc(1u, sizeof(SpoolerQueue));
            char          buffer[100];

            snprintf(buffer, sizeof(buffer), "ns:driver:writer:%d", i);
            Ns_MutexSetName2(&queuePtr->lock, buffer, "queue");
            queuePtr->id = i;
            Push(queuePtr, wrPtr->firstPtr);
        }
    } else {
        Ns_Log(Notice, "%s: enable %d writer thread(s) ",
               threadName, wrPtr->threads);
    }

    return NS_OK;
}