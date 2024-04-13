NsAsyncWriterQueueEnable(void)
{
    if (Ns_ConfigBool(NS_CONFIG_PARAMETERS, "asynclogwriter", NS_FALSE) == NS_TRUE) {
        SpoolerQueue  *queuePtr;

        /*
         * In case, the async writer has not started, the static variable
         * asyncWriter is NULL.
         */
        if (asyncWriter == NULL) {
            Ns_MutexLock(&reqLock);
            if (likely(asyncWriter == NULL)) {
                /*
                 * Allocate and initialize writer thread context.
                 */
                asyncWriter = ns_calloc(1u, sizeof(AsyncWriter));
                Ns_MutexUnlock(&reqLock);
                Ns_MutexSetName2(&asyncWriter->lock, "ns:driver", "async-writer");
                /*
                 * Allocate and initialize a Spooler Queue for this thread.
                 */
                queuePtr = ns_calloc(1u, sizeof(SpoolerQueue));
                Ns_MutexSetName2(&queuePtr->lock, "ns:driver:async-writer", "queue");
                asyncWriter->firstPtr = queuePtr;
                /*
                 * Start the spooler queue
                 */
                SpoolerQueueStart(queuePtr, AsyncWriterThread);

            } else {
                Ns_MutexUnlock(&reqLock);
            }
        }


        assert(asyncWriter != NULL);
        queuePtr = asyncWriter->firstPtr;
        assert(queuePtr != NULL);

        Ns_MutexLock(&queuePtr->lock);
        queuePtr->stopped = NS_FALSE;
        Ns_MutexUnlock(&queuePtr->lock);
    }
}