void *Jsi_HashKeyGet(Jsi_HashEntry *hPtr)
{
    Jsi_Hash *t = hPtr->tablePtr;
    return (t->keyType == JSI_KEYS_ONEWORD || t->keyType == JSI_KEYS_STRINGKEY ? hPtr->key.oneWordValue : hPtr->key.string);
}