SpoolerQueueStop(SpoolerQueue *queuePtr, const Ns_Time *timeoutPtr, const char *name)
{

    NS_NONNULL_ASSERT(timeoutPtr != NULL);
    NS_NONNULL_ASSERT(name != NULL);

    while (queuePtr != NULL) {
        Ns_ReturnCode status;

        Ns_MutexLock(&queuePtr->lock);
        if (!queuePtr->stopped && !queuePtr->shutdown) {
            Ns_Log(Debug, "%s%d: triggering shutdown", name, queuePtr->id);
            queuePtr->shutdown = NS_TRUE;
            SockTrigger(queuePtr->pipe[1]);
        }
        status = NS_OK;
        while (!queuePtr->stopped && status == NS_OK) {
            status = Ns_CondTimedWait(&queuePtr->cond, &queuePtr->lock, timeoutPtr);
        }
        if (status != NS_OK) {
            Ns_Log(Warning, "%s%d: timeout waiting for shutdown", name, queuePtr->id);
        } else {
            /*Ns_Log(Notice, "%s%d: shutdown complete", name, queuePtr->id);*/
            if (queuePtr->thread != NULL) {
                Ns_ThreadJoin(&queuePtr->thread, NULL);
                queuePtr->thread = NULL;
            } else {
                Ns_Log(Notice, "%s%d: shutdown: thread already gone", name, queuePtr->id);
            }
            ns_sockclose(queuePtr->pipe[0]);
            ns_sockclose(queuePtr->pipe[1]);
        }
        Ns_MutexUnlock(&queuePtr->lock);
        queuePtr = queuePtr->nextPtr;
    }
}