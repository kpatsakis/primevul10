static int OneWordCompare(Jsi_Tree *treePtr, const void *key1, const void *key2)
{
    return ((uintptr_t )key1 - (uintptr_t)key2);
}