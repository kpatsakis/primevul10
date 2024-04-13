static Jsi_TreeEntry *TreeStringCreate( Jsi_Tree *treePtr, const void *key, bool *newPtr)
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
    size = sizeof(Jsi_TreeEntry) + Jsi_Strlen((char*)key) /*- sizeof(jsi_TreeKey)*/ + 1;
    hPtr = (Jsi_TreeEntry*)Jsi_Calloc(1,size);
    SIGINIT(hPtr,TREEENTRY);
    hPtr->typ = JSI_MAP_TREE;
    hPtr->treePtr = treePtr;
    hPtr->value = 0;
    Jsi_Strcpy(hPtr->key.string, (char*)key);
    treePtr->numEntries++;
    return hPtr;
}