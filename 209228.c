NsStopDrivers(void)
{
    Driver *drvPtr;

    NsAsyncWriterQueueDisable(NS_TRUE);

    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
        Tcl_HashEntry  *hPtr;
        Tcl_HashSearch  search;

        if ((drvPtr->flags & DRIVER_STARTED) == 0u) {
            continue;
        }

        Ns_MutexLock(&drvPtr->lock);
        Ns_Log(Notice, "[driver:%s]: stopping", drvPtr->threadName);
        drvPtr->flags |= DRIVER_SHUTDOWN;
        Ns_CondBroadcast(&drvPtr->cond);
        Ns_MutexUnlock(&drvPtr->lock);
        SockTrigger(drvPtr->trigger[1]);

        hPtr = Tcl_FirstHashEntry(&drvPtr->hosts, &search);
        while (hPtr != NULL) {
            Tcl_DeleteHashEntry(hPtr);
            hPtr = Tcl_NextHashEntry(&search);
        }
    }
}