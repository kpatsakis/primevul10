static int StringPtrCompare(Jsi_Tree *treePtr, const void *key1, const void *key2)
{
    //return (key1 - key2);
    if (key1 == key2) return 0;
    //return Jsi_DictionaryCompare((char*)key1, (char*)key2);
    return Jsi_Strcmp((char*)key1, (char*)key2);
}