WriterGetInfoPtr(WriterSock *curPtr, Tcl_HashTable *pools)
{
    NS_NONNULL_ASSERT(curPtr != NULL);
    NS_NONNULL_ASSERT(pools != NULL);

    if (curPtr->infoPtr == NULL) {
        int            isNew;
        Tcl_HashEntry  *hPtr;

        hPtr = Tcl_CreateHashEntry(pools, (void*)curPtr->poolPtr, &isNew);
        if (isNew == 1) {
            /*
             * This is a pool that we have not seen yet.
             */
            curPtr->infoPtr = ns_malloc(sizeof(ConnPoolInfo));
            curPtr->infoPtr->currentPoolRate = 0;
            curPtr->infoPtr->threadSlot =
                NsPoolAllocateThreadSlot(curPtr->poolPtr, Ns_ThreadId());
            Tcl_SetHashValue(hPtr, curPtr->infoPtr);
            Ns_Log(DriverDebug, "poollimit: pool '%s' allocate infoPtr with slot %lu poolLimit %d",
                   curPtr->poolPtr->pool,
                   curPtr->infoPtr->threadSlot,
                   curPtr->poolPtr->rate.poolLimit);
        } else {
            curPtr->infoPtr = (ConnPoolInfo *)Tcl_GetHashValue(hPtr);
        }
    }

    return curPtr->infoPtr;
}