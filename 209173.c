NsStopSpoolers(void)
{
    const Driver *drvPtr;

    Ns_Log(Notice, "driver: stopping writer and spooler threads");

    /*
     * Shutdown all spooler and writer threads
     */
    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
        Ns_Time timeout;

        if ((drvPtr->flags & DRIVER_STARTED) == 0u) {
            continue;
        }
        Ns_GetTime(&timeout);
        Ns_IncrTime(&timeout, nsconf.shutdowntimeout.sec, nsconf.shutdowntimeout.usec);

        SpoolerQueueStop(drvPtr->writer.firstPtr, &timeout, "writer");
        SpoolerQueueStop(drvPtr->spooler.firstPtr, &timeout, "spooler");
    }
}