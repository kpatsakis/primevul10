HashStringCreate( Jsi_Hash *tablePtr, const void *key, bool *newPtr)
{
    jsi_Hash hval;
    Jsi_HashEntry **bucketPtr;
    Jsi_HashEntry *hPtr;
    size_t size, hindex;

    hval = HashString((const char*)key);
    hindex = hval & tablePtr->mask;

    for (hPtr = tablePtr->buckets[hindex]; hPtr != NULL;
            hPtr = hPtr->nextPtr) {
        if (hPtr->hval == hval) {
            const char *p1, *p2;

            for (p1 = (const char*)key, p2 = hPtr->key.string; ; p1++, p2++) {
                if (*p1 != *p2) {
                    break;
                }
                if (*p1 == '\0') {
                    if (newPtr)
                        *newPtr = 0;
                    return hPtr;
                }
            }
        }
    }

    if (newPtr)
        *newPtr = 1;
    size = sizeof(Jsi_HashEntry) + Jsi_Strlen((char*)key) - sizeof(jsi_HashKey) + 1;
    hPtr = (Jsi_HashEntry*)Jsi_Calloc(1, size);
    hPtr->typ = JSI_MAP_HASH;
    bucketPtr = tablePtr->buckets + hindex;
    hPtr->tablePtr = tablePtr;
    hPtr->nextPtr = *bucketPtr;
    hPtr->hval = hval;
    hPtr->clientData = 0;
    Jsi_Strcpy(hPtr->key.string, (char*)key);
    *bucketPtr = hPtr;
    tablePtr->numEntries++;

    if (tablePtr->numEntries >= tablePtr->rebuildSize)
        RebuildTable(tablePtr);
        
    return hPtr;
}