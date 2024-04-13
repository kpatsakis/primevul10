NsAsyncWriterQueueDisable(bool shutdown)
{
    if (asyncWriter != NULL) {
        SpoolerQueue *queuePtr = asyncWriter->firstPtr;
        Ns_Time       timeout;

        assert(queuePtr != NULL);

        Ns_GetTime(&timeout);
        Ns_IncrTime(&timeout, nsconf.shutdowntimeout.sec, nsconf.shutdowntimeout.usec);

        Ns_MutexLock(&queuePtr->lock);
        queuePtr->stopped = NS_TRUE;
        queuePtr->shutdown = shutdown;

        /*
         * Trigger the AsyncWriter Thread to drain the spooler queue.
         */
        SockTrigger(queuePtr->pipe[1]);
        (void)Ns_CondTimedWait(&queuePtr->cond, &queuePtr->lock, &timeout);

        Ns_MutexUnlock(&queuePtr->lock);

        if (shutdown) {
            ns_free(queuePtr);
            ns_free(asyncWriter);
            asyncWriter = NULL;
        }
    }
}