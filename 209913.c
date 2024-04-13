jsi_HashArrayCreate( Jsi_Hash *tablePtr,  const void *key, bool *newPtr)
{
    jsi_Hash hval = jsi_HashArray(key, tablePtr->keyType);
    size_t size, hindex = hval & tablePtr->mask;
    Jsi_HashEntry *hPtr = tablePtr->buckets[hindex];

    for (; hPtr != NULL; hPtr = hPtr->nextPtr)
        if (hPtr->hval == hval && !memcmp(hPtr->key.string, key, tablePtr->keyType)) {
            if (newPtr)
                *newPtr = 0;
            return hPtr;
        }

    /* Entry not found.  Add a new one to the bucket. */
    if (newPtr)
        *newPtr = 1;
    assert(tablePtr->keyType >= JSI_KEYS_STRUCT_MINSIZE);
    size = sizeof(Jsi_HashEntry) + tablePtr->keyType;
    if ((uint)tablePtr->keyType > sizeof(jsi_HashKey)) // Avoid memory checker problems by not truncating struct.
        size -= sizeof(jsi_HashKey);
    hPtr = (Jsi_HashEntry*)Jsi_Calloc(1, size);
    Jsi_HashEntry **bucketPtr = tablePtr->buckets + hindex;
    hPtr->typ = JSI_MAP_HASH;
    hPtr->tablePtr = tablePtr;
    hPtr->nextPtr = *bucketPtr;
    hPtr->hval = hval;
    hPtr->clientData = 0;
    memcpy(hPtr->key.string, key, tablePtr->keyType);
    *bucketPtr = hPtr;
    tablePtr->numEntries++;

    /*
     * If the table has exceeded a decent size, rebuild it with many
     * more buckets.
     */
    if (tablePtr->numEntries >= tablePtr->rebuildSize)
        RebuildTable(tablePtr);

    return hPtr;
}