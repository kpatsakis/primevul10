void *Jsi_HashGet(Jsi_Hash *tbl, const void *key, int flags) {
    Jsi_HashEntry *hPtr;
    hPtr = Jsi_HashEntryFind(tbl, key);
    if (!hPtr)
        return NULL;
    return Jsi_HashValueGet(hPtr);
}