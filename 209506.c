Jsi_HashClear(Jsi_Hash *tablePtr)
{
    Jsi_HashEntry *hPtr, *nextPtr;
    size_t i;
    if (!tablePtr)
        return;
    if (tablePtr->opts.lockHashProc && (*tablePtr->opts.lockHashProc)(tablePtr, 1) != JSI_OK)
        return;
    for (i = 0; i < (size_t)tablePtr->numBuckets; i++) {
        hPtr = tablePtr->buckets[i];
        while (hPtr != NULL) {
            nextPtr = hPtr->nextPtr;
            if (tablePtr->opts.freeHashProc && hPtr->clientData)
                (tablePtr->opts.freeHashProc)(tablePtr->opts.interp, hPtr, hPtr->clientData);
            Jsi_Free(hPtr);
            hPtr = nextPtr;
            tablePtr->buckets[i] = hPtr;
        }
    }
    
    if (tablePtr->buckets != tablePtr->staticBuckets) {
        Jsi_Free(tablePtr->buckets);
    }
    if (tablePtr->opts.lockHashProc)
        (*tablePtr->opts.lockHashProc)(tablePtr, 0);
}