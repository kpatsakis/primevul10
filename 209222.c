NsWaitDriversShutdown(const Ns_Time *toPtr)
{
    Driver       *drvPtr;
    Ns_ReturnCode status = NS_OK;

    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {
        if ((drvPtr->flags & DRIVER_STARTED) == 0u) {
            continue;
        }
        Ns_MutexLock(&drvPtr->lock);
        while ((drvPtr->flags & DRIVER_STOPPED) == 0u && status == NS_OK) {
            status = Ns_CondTimedWait(&drvPtr->cond, &drvPtr->lock, toPtr);
        }
        Ns_MutexUnlock(&drvPtr->lock);
        if (status != NS_OK) {
            Ns_Log(Warning, "[driver:%s]: shutdown timeout", drvPtr->threadName);
        } else {
            Ns_Log(Notice, "[driver:%s]: stopped", drvPtr->threadName);
            Ns_ThreadJoin(&drvPtr->thread, NULL);
            drvPtr->thread = NULL;
        }
    }
}