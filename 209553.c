bool Jsi_HashUnset(Jsi_Hash *tbl, const void *key) {
    Jsi_HashEntry *hPtr;
    hPtr = Jsi_HashEntryFind(tbl, key);
    if (!hPtr)
        return false;
    Jsi_HashEntryDelete(hPtr);
    return true;
}