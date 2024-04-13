void *Jsi_TreeKeyGet(Jsi_TreeEntry *hPtr)
{
    Jsi_Tree *t = hPtr->treePtr;
    return (t->keyType == JSI_KEYS_ONEWORD || t->keyType == JSI_KEYS_STRINGKEY ? hPtr->key.oneWordValue : hPtr->key.string);
}