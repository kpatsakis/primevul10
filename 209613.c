HashOneWordCreate( Jsi_Hash *tablePtr, const void *key, bool *newPtr)
{
    Jsi_HashEntry **bucketPtr;
     Jsi_HashEntry *hPtr;
    size_t hindex;

    hindex = RANDOM_INDEX(tablePtr, key);


    for (hPtr = tablePtr->buckets[hindex]; hPtr != NULL;
        hPtr = hPtr->nextPtr) {
        if (hPtr->key.oneWordValue == key) {
            if (newPtr)
                *newPtr = 0;
            return hPtr;
        }
    }

    if (newPtr)
        *newPtr = 1;
    hPtr = (Jsi_HashEntry*)Jsi_Calloc(1, sizeof(*hPtr));
    hPtr->typ = JSI_MAP_HASH;
    bucketPtr = tablePtr->buckets + hindex;
    hPtr->tablePtr = tablePtr;
    hPtr->nextPtr = *bucketPtr;
    hPtr->hval = (jsi_Hash)key;
    hPtr->clientData = 0;
    hPtr->key.oneWordValue = (void *)key; 
    *bucketPtr = hPtr;
    tablePtr->numEntries++;


    if (tablePtr->numEntries >= tablePtr->rebuildSize)
        RebuildTable(tablePtr);

    return hPtr;
}