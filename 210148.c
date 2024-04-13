Jsi_TreeEntry *Jsi_TreeSet(Jsi_Tree *treePtr, const void *key, void *value)
{
    Jsi_TreeEntry *hPtr;
    bool isNew;
    hPtr = Jsi_TreeEntryNew(treePtr, key, &isNew);
    if (!hPtr) return hPtr;
    Jsi_TreeValueSet(hPtr, value);
    return hPtr;
}