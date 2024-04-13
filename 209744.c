jsi_HashArrayFind( Jsi_Hash *tablePtr, const void *key)
{
    jsi_Hash hval = jsi_HashArray(key, tablePtr->keyType);
    size_t hindex = hval & tablePtr->mask;
    Jsi_HashEntry *hPtr = tablePtr->buckets[hindex];

    for (; hPtr != NULL; hPtr = hPtr->nextPtr)
        if (hPtr->hval == hval && !memcmp(hPtr->key.string, key, tablePtr->keyType))
            return hPtr;

    return NULL;
}