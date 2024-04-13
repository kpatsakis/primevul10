bool Jsi_TreeUnset(Jsi_Tree *treePtr, void *key) {
    Jsi_TreeEntry *hPtr = Jsi_TreeEntryFind(treePtr, key);
    if (!hPtr)
        return false;
    Jsi_TreeEntryDelete(hPtr);
    return true;
}