static int StringCompare(Jsi_Tree *treePtr, const void *key1, const void *key2)
{
    return Jsi_DictionaryCompare((char*)key1, (char*)key2);
    //return Jsi_Strcmp((char*)key1, (char*)key2);
}