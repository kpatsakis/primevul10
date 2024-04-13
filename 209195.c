DriverListen(Driver *drvPtr, const char *bindaddr)
{
    NS_SOCKET sock;

    NS_NONNULL_ASSERT(drvPtr != NULL);
    NS_NONNULL_ASSERT(bindaddr != NULL);

    sock = (*drvPtr->listenProc)((Ns_Driver *) drvPtr,
                                 bindaddr,
                                 drvPtr->port,
                                 drvPtr->backlog,
                                 drvPtr->reuseport);
    if (sock == NS_INVALID_SOCKET) {
        Ns_Log(Error, "%s: failed to listen on [%s]:%d: %s",
               drvPtr->threadName, bindaddr, drvPtr->port,
               ns_sockstrerror(ns_sockerrno));
    } else {
        Ns_Log(Notice,
#ifdef HAVE_IPV6
               "%s: listening on [%s]:%d",
#else
               "%s: listening on %s:%d",
#endif
               drvPtr->threadName, bindaddr, drvPtr->port);
    }

    return sock;
}