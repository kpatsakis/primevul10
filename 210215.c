static Jsi_TreeEntry *StringPtrCreate( Jsi_Tree *treePtr, const void *key, bool *newPtr)
{
    return OneWordCreate(treePtr, key, newPtr);
}