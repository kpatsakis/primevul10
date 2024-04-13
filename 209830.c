Jsi_TreeEntry *Jsi_TreeSearchFirst (Jsi_Tree *treePtr, Jsi_TreeSearch *searchPtr, int flags, const void *startKey)
{
    Jsi_TreeEntry *hPtr = NULL, *hPtr2 = NULL;
    if (!treePtr) return NULL;
    memset(searchPtr, 0, sizeof(*searchPtr));
    searchPtr->treePtr = treePtr;
    searchPtr->flags = flags;
    searchPtr->Ptrs = searchPtr->staticPtrs;
    searchPtr->max = sizeof(searchPtr->staticPtrs)/sizeof(searchPtr->staticPtrs[0]);
    searchPtr->epoch = treePtr->epoch;
    if (startKey || (flags & JSI_TREE_SEARCH_KEY))
        hPtr2 = Jsi_TreeEntryFind(treePtr, startKey);;
    searchPtr->current = treePtr->root;
    hPtr = searchAdd(searchPtr, treePtr->root);
    if (hPtr2 && hPtr && hPtr2 != hPtr)
        while (hPtr && hPtr2 != hPtr) // TODO: need a more efficient way to do this...
            hPtr = Jsi_TreeSearchNext(searchPtr);
    return hPtr;
}