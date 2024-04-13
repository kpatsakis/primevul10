Jsi_HashEntry* Jsi_HashSet(Jsi_Hash *tbl, const void *key, void *value) {
    Jsi_HashEntry *hPtr;
    bool isNew;
    hPtr = Jsi_HashEntryNew(tbl, key, &isNew);
    if (!hPtr) return hPtr;
    Jsi_HashValueSet(hPtr, value);
    return hPtr;
}