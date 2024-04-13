WriterPerPoolRates(WriterSock *writePtr, Tcl_HashTable *pools)
{
    WriterSock     *curPtr;
    Tcl_HashSearch  search;
    Tcl_HashEntry  *hPtr;

    NS_NONNULL_ASSERT(writePtr != NULL);
    NS_NONNULL_ASSERT(pools != NULL);

    /*
     * First reset pool total rate.  We keep the bandwidth managed pools in a
     * thread-local memory. Before, we accumulate the data, we reset it.
     */
    hPtr = Tcl_FirstHashEntry(pools, &search);
    while (hPtr != NULL) {
        ConnPoolInfo *infoPtr = (ConnPoolInfo *)Tcl_GetHashValue(hPtr);
        infoPtr->currentPoolRate = 0;
        hPtr = Tcl_NextHashEntry(&search);
    }

    /*
     * Sum the actual rates per bandwidth limited pool for all active writer
     * jobs.
     */
    for (curPtr = writePtr; curPtr != NULL; curPtr = curPtr->nextPtr) {
        /*
         * Does the writer come form a badwidth limited pool?
         */
        if (curPtr->poolPtr->rate.poolLimit > 0 && curPtr->currentRate > 0) {
            /*
             * Add the actual rate to the writer specific pool rate.
             */
            ConnPoolInfo *infoPtr = WriterGetInfoPtr(curPtr, pools);

            infoPtr->currentPoolRate += curPtr->currentRate;
            Ns_Log(DriverDebug, "poollimit pool '%s' added rate poolLimit %d poolRate %d",
                   curPtr->poolPtr->pool,
                   curPtr->poolPtr->rate.poolLimit,
                   infoPtr->currentPoolRate);
        }
    }

    /*
     * Now iterate over the pools used by this thread and sum the specific
     * pool rates from all writer threads.
     */
    hPtr = Tcl_FirstHashEntry(pools, &search);
    while (hPtr != NULL) {
        ConnPool     *poolPtr = (ConnPool *)Tcl_GetHashKey(pools, hPtr);
        int           totalPoolRate, writerThreadCount, threadDeltaRate;
        ConnPoolInfo *infoPtr;

        /*
         * Compute the following indicators:
         *   - totalPoolRate: accumulated pool rates from all writer threads.
         *
         *   - threadDeltaRate: how much of the available bandwidth can i used
         *     the current thread. We assume that the distribution of writers
         *     between all writer threads is even, so we can split the
         *     available rate by the number of writer threads working on this
         *     pool.
         *
         *  - deltaPercentage: adjust in a single iteration just a fraction
         *    (e.g. 10 percent) of the potential change. This function is
         *    called often enough to justify delayed adjustments.
         */
        infoPtr = (ConnPoolInfo *)Tcl_GetHashValue(hPtr);
        totalPoolRate = NsPoolTotalRate(poolPtr,
                                        infoPtr->threadSlot,
                                        infoPtr->currentPoolRate,
                                        &writerThreadCount);

        /*
         * If nothing is going on, allow a thread the full rate.
         */
        if (infoPtr->currentPoolRate == 0) {
            threadDeltaRate = (poolPtr->rate.poolLimit - totalPoolRate);
        } else {
            threadDeltaRate = (poolPtr->rate.poolLimit - totalPoolRate) / writerThreadCount;
        }
        infoPtr->deltaPercentage = threadDeltaRate / 10;
        if (infoPtr->deltaPercentage < -50) {
            infoPtr->deltaPercentage = -50;
        }

        if (totalPoolRate > 0) {
            Ns_Log(Notice, "... pool '%s' thread's pool rate %d total pool rate %d limit %d "
                   "(#%d writer threads) -> computed rate %d (%d%%) ",
                   NsPoolName(poolPtr->pool),
                   infoPtr->currentPoolRate,
                   totalPoolRate,
                   poolPtr->rate.poolLimit,
                   writerThreadCount,
                   threadDeltaRate,
                   infoPtr->deltaPercentage
                   );
        }

        hPtr = Tcl_NextHashEntry(&search);
    }
}