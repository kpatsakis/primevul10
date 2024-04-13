NsStartDrivers(void)
{
    Driver *drvPtr;

    /*
     * Signal and wait for each driver to start.
     */
    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {

        if (drvPtr->port == 0u) {
            /*
             * Don't start a driver having port zero.
             */
            continue;
        }

        Ns_ThreadCreate(DriverThread, drvPtr, 0, &drvPtr->thread);
        Ns_MutexLock(&drvPtr->lock);
        while ((drvPtr->flags & DRIVER_STARTED) == 0u) {
            Ns_CondWait(&drvPtr->cond, &drvPtr->lock);
        }
        /*if ((drvPtr->flags & DRIVER_FAILED)) {
          status = NS_ERROR;
          }*/
        Ns_MutexUnlock(&drvPtr->lock);
    }
}