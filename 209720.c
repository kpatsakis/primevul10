Jsi_HashSearchNext(Jsi_HashSearch *searchPtr)
{
    Jsi_HashEntry *hPtr;
    Jsi_Hash *tablePtr = searchPtr->tablePtr;
    int locked = 0;
    
    while (searchPtr->nextEntryPtr == NULL) {
        if (searchPtr->nextIndex >= (size_t)tablePtr->numBuckets) {
            if (tablePtr->opts.lockHashProc && locked)
                (*tablePtr->opts.lockHashProc)(tablePtr, 0);
            return NULL;
        }
        if (tablePtr->opts.lockHashProc && locked == 0 && (*tablePtr->opts.lockHashProc)(tablePtr, locked++) != JSI_OK)
            return NULL;
        searchPtr->nextEntryPtr =
            tablePtr->buckets[searchPtr->nextIndex];
        searchPtr->nextIndex++;
    }
    if (tablePtr->opts.lockHashProc && locked)
        (*tablePtr->opts.lockHashProc)(tablePtr, 0);
    hPtr = searchPtr->nextEntryPtr;
    searchPtr->nextEntryPtr = hPtr->nextPtr;
    return hPtr;
}