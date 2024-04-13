HashOneWordFind( Jsi_Hash *tablePtr,  const void *key)
{
     Jsi_HashEntry *hPtr;
    size_t hindex;

    hindex = RANDOM_INDEX(tablePtr, key);

    /*
     * Search all of the entries in the appropriate bucket.
     */
    for (hPtr = tablePtr->buckets[hindex]; hPtr != NULL;
        hPtr = hPtr->nextPtr) {
    if (hPtr->key.oneWordValue == key) {
        return hPtr;
    }
    }
    return NULL;
}