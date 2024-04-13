RebuildTable(Jsi_Hash *tablePtr)
{
    Jsi_HashEntry **bucketPtr, **oldBuckets;
    Jsi_HashEntry **oldChainPtr, **endPtr;
    Jsi_HashEntry *hPtr, *nextPtr;
    size_t hindex;

    oldBuckets = tablePtr->buckets;
    endPtr = tablePtr->buckets + tablePtr->numBuckets;
    tablePtr->numBuckets <<= 2;
    tablePtr->buckets = (Jsi_HashEntry**)Jsi_Calloc(tablePtr->numBuckets, 
                   sizeof(Jsi_HashEntry *));
    tablePtr->rebuildSize <<= 2;
    tablePtr->downShift -= 2;
    tablePtr->mask = tablePtr->numBuckets - 1;

 
    if (tablePtr->keyType == JSI_KEYS_ONEWORD) {

        for (oldChainPtr = oldBuckets; oldChainPtr < endPtr; oldChainPtr++) {
            for (hPtr = *oldChainPtr; hPtr != NULL; hPtr = nextPtr) {
            nextPtr = hPtr->nextPtr;
            hindex = RANDOM_INDEX(tablePtr, hPtr->key.oneWordValue);
            bucketPtr = tablePtr->buckets + hindex;
            hPtr->nextPtr = *bucketPtr;
            *bucketPtr = hPtr;
            }
        }
    } else {
        for (oldChainPtr = oldBuckets; oldChainPtr < endPtr; oldChainPtr++) {
            for (hPtr = *oldChainPtr; hPtr != NULL; hPtr = nextPtr) {
            nextPtr = hPtr->nextPtr;
            hindex = hPtr->hval & tablePtr->mask;
            bucketPtr = tablePtr->buckets + hindex;
            hPtr->nextPtr = *bucketPtr;
            *bucketPtr = hPtr;
            }
        }
    }

    if (oldBuckets != tablePtr->staticBuckets) {
        Jsi_Free(oldBuckets);
    }
}