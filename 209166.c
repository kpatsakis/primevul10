SpoolerQueueStart(SpoolerQueue *queuePtr, Ns_ThreadProc *proc)
{
    NS_NONNULL_ASSERT(proc != NULL);

    while (queuePtr != NULL) {
        if (ns_sockpair(queuePtr->pipe) != 0) {
            Ns_Fatal("ns_sockpair() failed: %s", ns_sockstrerror(ns_sockerrno));
        }
        Ns_ThreadCreate(proc, queuePtr, 0, &queuePtr->thread);
        queuePtr = queuePtr->nextPtr;
    }
}