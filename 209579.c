Jsi_TreeEntry *Jsi_TreeSearchNext(Jsi_TreeSearch *searchPtr)
{
    Jsi_TreeEntry *hPtr = NULL;
    if (searchPtr->epoch == searchPtr->treePtr->epoch)
        hPtr = searchAdd(searchPtr, NULL);
    if (!hPtr)
        Jsi_TreeSearchDone(searchPtr);
    return hPtr;
}