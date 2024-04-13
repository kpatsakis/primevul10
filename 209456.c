void *Jsi_TreeGet(Jsi_Tree *treePtr, void *key, int flags)
{
    Jsi_TreeEntry *hPtr = Jsi_TreeEntryFind(treePtr, key);
    if (!hPtr)
        return NULL;
    return Jsi_TreeValueGet(hPtr);
}