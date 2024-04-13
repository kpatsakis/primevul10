HashStringFind( Jsi_Hash *tablePtr, const void *key)
{
    jsi_Hash hval;
    Jsi_HashEntry *hPtr;
    size_t hindex;

    hval = HashString((char *)key);
    hindex = hval & tablePtr->mask;

    /*
     * Search all of the entries in the appropriate bucket.
     */

    for (hPtr = tablePtr->buckets[hindex]; hPtr != NULL;
            hPtr = hPtr->nextPtr) {
        if (hPtr->hval == hval) {
            const char *p1, *p2;

            for (p1 = (char*)key, p2 = hPtr->key.string; ; p1++, p2++) {
                if (*p1 != *p2) {
                    break;
                }
                if (*p1 == '\0') {
                    return hPtr;
                }
            }
        }
    }
    return NULL;
}