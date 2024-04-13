static Jsi_TreeEntry *OneWordCreate( Jsi_Tree *treePtr, const void *key, bool *newPtr)
{
    Jsi_TreeEntry *hPtr;
    size_t size;
    if ((hPtr = Jsi_TreeEntryFind(treePtr, key))) {
        if (newPtr)
            *newPtr = 0;
        return hPtr;
    }
    if (newPtr)
        *newPtr = 1;
    size = sizeof(Jsi_TreeEntry);
    hPtr = (Jsi_TreeEntry*)Jsi_Calloc(1,size);
    SIGINIT(hPtr,TREEENTRY);
    hPtr->typ = JSI_MAP_TREE;
    hPtr->treePtr = treePtr;
    hPtr->value = 0;
    hPtr->key.oneWordValue = (void *)key;
    treePtr->numEntries++;
    return hPtr;
}