static int TreeArrayCompare(Jsi_Tree *treePtr, const void *key1, const void *key2)
{
    return memcmp(key1, key2, treePtr->keyType);
}