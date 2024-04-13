Jsi_HashEntryDelete(Jsi_HashEntry *entryPtr)
{
    Jsi_HashEntry *prevPtr;
    Jsi_HashEntry **bucketPtr;
    size_t hindex;
    Jsi_Hash *tablePtr = entryPtr->tablePtr;
    Jsi_Interp *interp = tablePtr->opts.interp;
    JSI_NOTUSED(interp);
    int cnt = 0;
#ifdef JSI_HAS_SIG_HASHENTRY
    SIGASSERT(entryPtr, HASHENTRY);
#endif
    if (tablePtr->opts.lockHashProc && (*tablePtr->opts.lockHashProc)(tablePtr, 1) != JSI_OK)
        return 0;
    if (tablePtr->keyType == JSI_KEYS_ONEWORD) {
        hindex = RANDOM_INDEX(tablePtr, (const void *)entryPtr->hval);
    } else {
        hindex = (entryPtr->hval & tablePtr->mask);
    }
    bucketPtr = tablePtr->buckets + hindex;
    if (*bucketPtr == entryPtr) {
        *bucketPtr = entryPtr->nextPtr;
        cnt++;
    } else {
        for (prevPtr = *bucketPtr; /*empty*/; prevPtr = prevPtr->nextPtr) {
            if (prevPtr == NULL) {
                Jsi_LogBug("malformed bucket chain in Jsi_HashEntryDelete");
            }
            if (prevPtr->nextPtr == entryPtr) {
                prevPtr->nextPtr = entryPtr->nextPtr;
                cnt++;
                break;
            }
        }
    }
    if (tablePtr->opts.freeHashProc && entryPtr->clientData)
        (tablePtr->opts.freeHashProc)(tablePtr->opts.interp, entryPtr, entryPtr->clientData);

    if (tablePtr->opts.lockHashProc)
        (*tablePtr->opts.lockHashProc)(tablePtr, 0);
    tablePtr->numEntries--;
    Jsi_Free(entryPtr);
    return cnt;
}