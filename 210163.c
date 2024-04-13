static Jsi_TreeEntry *TreeArrayCreate(Jsi_Tree *treePtr, const void *key, bool *newPtr)
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
    size = sizeof(Jsi_TreeEntry) + treePtr->keyType; /*- sizeof(jsi_TreeKey);*/
    hPtr = (Jsi_TreeEntry*)Jsi_Calloc(1,size);
    SIGINIT(hPtr,TREEENTRY);
    hPtr->typ = JSI_MAP_TREE;
    hPtr->treePtr = treePtr;
    hPtr->value = 0;
    memcpy(hPtr->key.string, key, treePtr->keyType);
    treePtr->numEntries++;
    return hPtr;
}